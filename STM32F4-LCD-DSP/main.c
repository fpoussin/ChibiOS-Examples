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
#include "arm_math.h"

#define FFT_SIZE 256
#define SIN_TABLE_SIZE 2048
#define TWO_PI (M_PI * 2.0f)

const q15_t phaseIncrement = TWO_PI/SIN_TABLE_SIZE/8;
q15_t currentPhase = 0.0;

q15_t samples [SIN_TABLE_SIZE];
q15_t input_tmp[SIN_TABLE_SIZE];
q15_t testOutput[FFT_SIZE];

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
const uint32_t ifftFlag = 0;
const uint32_t doBitReverse = 1;

int uitoa(unsigned int value, char * buf, int max);

static WORKING_AREA(waThread2, 16384);
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

	const uint16_t width = gdispGetWidth();
	const uint16_t height = gdispGetHeight();

	const font_t font1 = gdispOpenFont("UI2 Double");

	uint32_t cnt, us;
	char time_str[8];
	char idx_str[8];

	arm_cfft_radix4_instance_q15 S;
	q15_t maxValue;
	uint32_t maxIndex = 0;

	/* Initialize the CFFT/CIFFT module */
	arm_cfft_radix4_init_q15(&S, FFT_SIZE, ifftFlag, doBitReverse);

	while (TRUE) {

		int i;
		for (i=0 ; i < SIN_TABLE_SIZE ; i+=2)
		{
			samples[i] = arm_sin_q15(currentPhase += phaseIncrement);
			samples[i+1] = 0.0f;
		}

		gdispClear(Blue);
		memset(testOutput, 0, FFT_SIZE);

		cnt = halGetCounterValue();
		/* Process the data through the CFFT/CIFFT module */
		arm_cfft_radix4_q15(&S, samples);

		/* Process the data through the Complex Magnitude Module for
		calculating the magnitude at each bin */
		arm_cmplx_mag_q15(samples, testOutput, FFT_SIZE);

		cnt = halGetCounterValue() - cnt;
		memset (time_str, 0, sizeof(time_str));
		us = cnt / 168;
		uitoa(us, time_str, sizeof(time_str));
		/* Calculates maxValue and returns corresponding BIN value */
		arm_max_q15(testOutput, FFT_SIZE, &maxValue, &maxIndex);

		uitoa(maxIndex, idx_str, sizeof(idx_str));

		for (i=1 ; i < FFT_SIZE && i < width ; i++)
		{
			gdispFillArea(i*2, 0, 2, testOutput[i]*3, White);
		}

		gdispDrawString(width-gdispGetStringWidth(time_str, font1)-3, height-24, time_str, font1, Grey);
		gdispDrawString(width-gdispGetStringWidth(idx_str, font1)-3, height-48, idx_str, font1, Grey);

		chThdSleepMilliseconds(2000);
	}
}

static WORKING_AREA(waThread1, 128);
__attribute__ ((__noreturn__))
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
    chThdSleepMilliseconds(500);
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
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
