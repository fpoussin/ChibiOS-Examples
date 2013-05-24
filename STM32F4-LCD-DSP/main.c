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

#define FFT_SIZE 256 // 4096-1024-256-64-16 lengths supported by DSP library
#define SIN_TABLE_SIZE 2048
#define TWO_PI (M_PI * 2.0f)
#define SAMPLING_RATE 48000

int16_t phaseIncrement = 0;
int16_t currentPhase = 0;

int16_t samples[SIN_TABLE_SIZE];
int16_t input_tmp[SIN_TABLE_SIZE];
int16_t testOutput[FFT_SIZE];
extern const float32_t testInput_f32_10khz[2048];

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
	char freq_str[8];

	arm_cfft_radix4_instance_q15 S;
	int16_t maxValue;
	uint32_t maxIndex = 0;

	/* Initialize the CFFT/CIFFT module */
	arm_cfft_radix4_init_q15(&S, FFT_SIZE, ifftFlag, doBitReverse);

	while (TRUE) {

		int i;

		phaseIncrement += 500;
		if (phaseIncrement > 30000) phaseIncrement = 0;
		currentPhase = 0.0;
		for (i=0 ; i < SIN_TABLE_SIZE/2 ; i++)
		{
			samples[i*2] = arm_sin_q15(currentPhase += phaseIncrement);
			samples[(i*2)+1] = 0;
		}

		//arm_float_to_q15((float32_t*)testInput_f32_10khz, samples, SIN_TABLE_SIZE);

		gdispClear(Blue);
		memset(testOutput, 0, FFT_SIZE);

		cnt = halGetCounterValue();
		/* Process the data through the CFFT/CIFFT module */
		arm_cfft_radix4_q15(&S, samples);

		/* Process the data through the Complex Magnitude Module for
		calculating the magnitude at each bin */
		arm_cmplx_mag_q15(samples, testOutput, FFT_SIZE/2); // We only care about the first half

		cnt = halGetCounterValue() - cnt;
		us = cnt / 168; // 168mhz = 168 instructions per us
		uitoa(us, time_str, sizeof(time_str));
		/* Calculates maxValue and returns corresponding BIN value */
		arm_max_q15(testOutput, FFT_SIZE/2, &maxValue, &maxIndex); // We only care about the first half

		uitoa((SAMPLING_RATE*maxIndex)/FFT_SIZE, freq_str, sizeof(freq_str));

		uint16_t s;
		for (i=1 ; i < FFT_SIZE/2 && i < width ; i++) // We only display the first half as the rest is useless.
		{
			s = testOutput[i] >> 4;
			gdispFillArea(i*4, height-s, 4, s, White);
		}

		gdispDrawString(0, 0, "FFT+MAG Processing time in us", font1, White);
		gdispDrawString(width-gdispGetStringWidth(time_str, font1)-3, 0, time_str, font1, White);

		gdispDrawString(0, 24, "Peak freq in HZ", font1, White);
		gdispDrawString(width-gdispGetStringWidth(freq_str, font1)-3, 24, freq_str, font1, White);

		chThdSleepMilliseconds(50);
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
