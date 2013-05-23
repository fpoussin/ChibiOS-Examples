/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "gfx.h"
#include "stdlib.h"
#include "string.h"
#include "test.h"
#include "math.h"
#include "arm_math.h"

#define TEST_LENGTH_SAMPLES 2048

/* -------------------------------------------------------------------
* External Input and Output buffer Declarations for FFT Bin Example
* ------------------------------------------------------------------- */
extern float32_t testInput_f32_10khz[TEST_LENGTH_SAMPLES];
static float32_t testOutput[TEST_LENGTH_SAMPLES/2];

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
uint32_t fftSize = 1024;
uint32_t ifftFlag = 0;
uint32_t doBitReverse = 1;

/* Reference index at which max energy of bin ocuurs */
uint32_t refIndex = 213, testIndex = 0;


int uitoa(unsigned int value, char * buf, int max) {
	int n = 0;
	int i = 0;
	unsigned int tmp = 0;

	if (NULL == buf) {
		return -3;
	}

	if (2 > max) {
		return -4;
	}

	i=1;
	tmp = value;
	for (;;) {
		tmp /= 10;
		if (0 >= tmp) {
			break;
		}
		i++;
	}
	if (i >= max) {
		buf[0] = '?';
		buf[1] = 0x0;
		return 2;
	}

	n = i;
	tmp = value;
	buf[i--] = 0x0;
	for (;;) {
		buf[i--] = (tmp % 10) + '0';
		tmp /= 10;
		if (0 >= tmp) {
			break;
		}
	}
	if (-1 != i) {
		buf[i--] = '-';
	}
	return n;
}

static WORKING_AREA(waThread2, 2048);
__attribute__ ((__noreturn__))
static msg_t Thread2(void *arg)  {

	(void)arg;
	chRegSetThreadName("lcd");

	/* Reset */
	palClearPad(GPIOC, GPIOC_PIN6);
	chThdSleepMilliseconds(10);
	palSetPad(GPIOC, GPIOC_PIN6);
	chThdSleepMilliseconds(10);

	gdispInit();
	gdispClear(Black);

	uint16_t width = gdispGetWidth();
	uint16_t height = gdispGetHeight();

	const font_t font1 = gdispOpenFont("UI2 Double");
	const char *msg = "ChibiOS/GFX on SSD1963";

	const char *dsp_success = "FFT SUCCESS";
	const char *dsp_fail = "FFT FAILED";

	arm_status status;
	arm_cfft_radix4_instance_f32 S;
	float32_t maxValue;

	status = ARM_MATH_SUCCESS;

	while (TRUE) {
		gdispClear(Black);

		/* Initialize the CFFT/CIFFT module */
		status = arm_cfft_radix4_init_f32(&S, fftSize, ifftFlag, doBitReverse);

		/* Process the data through the CFFT/CIFFT module */
		arm_cfft_radix4_f32(&S, testInput_f32_10khz);

		/* Process the data through the Complex Magnitude Module for
		calculating the magnitude at each bin */
		arm_cmplx_mag_f32(testInput_f32_10khz, testOutput, fftSize);

		/* Calculates maxValue and returns corresponding BIN value */
		arm_max_f32(testOutput, fftSize, &maxValue, &testIndex);

		if(testIndex !=  refIndex)
		{
			status = ARM_MATH_TEST_FAILURE;
		}

		if( status != ARM_MATH_SUCCESS)
		{
			gdispDrawString(width-gdispGetStringWidth(dsp_fail, font1)-3, height/2, dsp_fail, font1, White);
		}
		else
		{
			gdispDrawString(width-gdispGetStringWidth(dsp_success, font1)-3, height/2, dsp_success, font1, White);
		}

		gdispDrawString(width-gdispGetStringWidth(msg, font1)-3, height-24, msg, font1, White);

		chThdSleepMilliseconds(2000);

	}
}

/*
 * Application entry point.
 */
__attribute__ ((__noreturn__))
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Creates the example thread.
   */
//  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
