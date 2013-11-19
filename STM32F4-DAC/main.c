/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"

/*
 * DAC test data (sine wave) - size: 360 half words, channels shifted by 180 degrees. (inverted)
 * Real data needs to have both left and right data after each other (left right left right, etc) in 32b sequences (16 per channel)
 * DAC Channel 2 is left.
 */
 
 const dacsample_t dac_data[] = {32767, 32767, 33910, 31624, 35052, 30482, 36192, 29342, 37327, 
	 28207, 38456, 27078, 39579, 25955, 40694, 24840, 41798, 23736, 42892, 22642, 43973, 21561, 
	 45041, 20493, 46094, 19440, 47131, 18403, 48150, 17384, 49150, 16384, 50130, 15404, 51090, 
	 14444, 52026, 13508, 52940, 12594, 53829, 11705, 54692, 10842, 55528, 10006, 56337, 9197, 
	 57117, 8417, 57867, 7667, 58587, 6947, 59276, 6258, 59932, 5602, 60554, 4980, 61144, 4390, 
	 61698, 3836, 62217, 3317, 62701, 2833, 63148, 2386, 63557, 1977, 63930, 1604, 64264, 1270, 
	 64560, 974, 64817, 717, 65036, 498, 65215, 319, 65354, 180, 65454, 80, 65514, 20, 65534, 0, 
	 65514, 20, 65454, 80, 65354, 180, 65215, 319, 65036, 498, 64817, 717, 64560, 974, 64264, 1270, 
	 63930, 1604, 63557, 1977, 63148, 2386, 62701, 2833, 62217, 3317, 61698, 3836, 61144, 4390, 
	 60554, 4980, 59932, 5602, 59276, 6258, 58587, 6947, 57867, 7667, 57117, 8417, 56337, 9197, 
	 55528, 10006, 54692, 10842, 53829, 11705, 52940, 12594, 52026, 13508, 51090, 14444, 50130, 
	 15404, 49150, 16384, 48150, 17384, 47131, 18403, 46094, 19440, 45041, 20493, 43973, 21561, 
	 42892, 22642, 41798, 23736, 40694, 24840, 39579, 25955, 38456, 27078, 37327, 28207, 36192, 
	 29342, 35052, 30482, 33910, 31624, 32767, 32767, 31624, 33910, 30482, 35052, 29342, 36192, 
	 28207, 37327, 27078, 38456, 25955, 39579, 24840, 40694, 23736, 41798, 22642, 42892, 21561, 
	 43973, 20493, 45041, 19440, 46094, 18403, 47131, 17384, 48150, 16384, 49150, 15404, 50130, 
	 14444, 51090, 13508, 52026, 12594, 52940, 11705, 53829, 10842, 54692, 10006, 55528, 9197, 
	 56337, 8417, 57117, 7667, 57867, 6947, 58587, 6258, 59276, 5602, 59932, 4980, 60554, 4390, 
	 61144, 3836, 61698, 3317, 62217, 2833, 62701, 2386, 63148, 1977, 63557, 1604, 63930, 1270, 
	 64264, 974, 64560, 717, 64817, 498, 65036, 319, 65215, 180, 65354, 80, 65454, 20, 65514, 0, 
	 65534, 20, 65514, 80, 65454, 180, 65354, 319, 65215, 498, 65036, 717, 64817, 974, 64560, 1270, 
	 64264, 1604, 63930, 1977, 63557, 2386, 63148, 2833, 62701, 3317, 62217, 3836, 61698, 4390, 
	 61144, 4980, 60554, 5602, 59932, 6258, 59276, 6947, 58587, 7667, 57867, 8417, 57117, 9197, 
	 56337, 10006, 55528, 10842, 54692, 11705, 53829, 12594, 52940, 13508, 52026, 14444, 51090, 
	 15404, 50130, 16384, 49150, 17384, 48150, 18403, 47131, 19440, 46094, 20493, 45041, 21561, 
	 43973, 22642, 42892, 23736, 41798, 24840, 40694, 25955, 39579, 27078, 38456, 28207, 37327,
	 29342, 36192, 30482, 35052, 31624, 33910};

		
dacsample_t dac_buffer_lr[sizeof(dac_data)*2];

/*
 * Turn on the orange LED once DMA ends the transmission.
 */
static void daccb(DACDriver *dacp, const dacsample_t * samples, size_t pos) {
  (void)dacp;
  (void)pos; /* pos can be half or full size of the buffer */
  palTogglePad(GPIOD, GPIOD_LED3); // Orange
	
  /* samples is at the beginning or half of the buffer depending on which DMA interrupt is generated (half or full)
       we can use it in both cases.	*/
  dmaStartMemCopy(STM32_DMA2_STREAM7, 
    STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD,
    dac_data, samples, sizeof(dac_data)/2);
}

/*
 * Turn on the red LED if there are DMA errors.
 */
static void dacerrcb(DACDriver *dacp, uint32_t flags) {
  (void)dacp;
  (void)flags;
  palTogglePad(GPIOD, GPIOD_LED4); // Red
}

/*
 * DAC conversion group, with callbacks.
 */
static const DACConversionGroup dacconvgrp = {
  2, /* Channels */
  daccb, /* End of transfer callback */
  dacerrcb, /* Error callback */
  true /*circular mode */ 
};

/*
 * DAC config
 */
static const DACConfig daccfg = {
  22050, /* Multiply the buffer size to the desired frequency in Hz */
  DAC_DHRM_12BIT_LEFT_DUAL, /* data holding register mode */
  0 /* DAC CR flags */
};

/*
 * Red LEDs blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palSetPad(GPIOD, GPIOD_LED5);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOD, GPIOD_LED5);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Application entry point.
 */
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
   * Setting up analog outputs used by the demo.
   */
  palSetGroupMode(GPIOA, PAL_PORT_BIT(4) | PAL_PORT_BIT(5), 0, PAL_MODE_INPUT_ANALOG);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Starting the DAC driver.
   */
  dacStart(&DACD1, &daccfg);
  dacStart(&DACD2, &daccfg);
  
  /* Allocating two DMA2 streams for memory copy operations.*/
  if (dmaStreamAllocate(STM32_DMA2_STREAM7, 0, NULL, NULL))
    chSysHalt();
  
  /*
   * Sending the dac_buffer
   */
  dacStartConversion(&DACD1, &dacconvgrp, dac_buffer_lr, sizeof(dac_buffer_lr));

  /*
   * Normal main() thread activity, in this demo it does nothing.
   */
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
