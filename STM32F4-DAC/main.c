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

#define DAC_BUFFER_SIZE 360

/*
 * DAC test buffer (sine wave)
 * Real buffer needs to have both left and right data after each other (left right left right, etc) in 32b sequences (16 per channel)
 */
const uint16_t dac_buffer[DAC_BUFFER_SIZE] = {2047, 2082, 2118, 2154, 2189, 2225, 2260,
		2296, 2331, 2367, 2402, 2437, 2472, 2507, 2542, 2576, 2611, 2645, 2679, 2713,
		2747, 2780, 2813, 2846, 2879, 2912, 2944, 2976, 3008, 3039, 3070, 3101, 3131,
		3161, 3191, 3221, 3250, 3278, 3307, 3335, 3362, 3389, 3416, 3443, 3468, 3494,
		3519, 3544, 3568, 3591, 3615, 3637, 3660, 3681, 3703, 3723, 3744, 3763, 3782,
		3801, 3819, 3837, 3854, 3870, 3886, 3902, 3917, 3931, 3944, 3958, 3970, 3982,
		3993, 4004, 4014, 4024, 4033, 4041, 4049, 4056, 4062, 4068, 4074, 4078, 4082,
		4086, 4089, 4091, 4092, 4093, 4094, 4093, 4092, 4091, 4089, 4086, 4082, 4078,
		4074, 4068, 4062, 4056, 4049, 4041, 4033, 4024, 4014, 4004, 3993, 3982, 3970,
		3958, 3944, 3931, 3917, 3902, 3886, 3870, 3854, 3837, 3819, 3801, 3782, 3763,
		3744, 3723, 3703, 3681, 3660, 3637, 3615, 3591, 3568, 3544, 3519, 3494, 3468,
		3443, 3416, 3389, 3362, 3335, 3307, 3278, 3250, 3221, 3191, 3161, 3131, 3101,
		3070, 3039, 3008, 2976, 2944, 2912, 2879, 2846, 2813, 2780, 2747, 2713, 2679,
		2645, 2611, 2576, 2542, 2507, 2472, 2437, 2402, 2367, 2331, 2296, 2260, 2225,
		2189, 2154, 2118, 2082, 2047, 2012, 1976, 1940, 1905, 1869, 1834, 1798, 1763,
		1727, 1692, 1657, 1622, 1587, 1552, 1518, 1483, 1449, 1415, 1381, 1347, 1314,
		1281, 1248, 1215, 1182, 1150, 1118, 1086, 1055, 1024, 993, 963, 933, 903, 873,
		844, 816, 787, 759, 732, 705, 678, 651, 626, 600, 575, 550, 526, 503, 479, 457,
		434, 413, 391, 371, 350, 331, 312, 293, 275, 257, 240, 224, 208, 192, 177, 163,
		150, 136, 124, 112, 101, 90, 80, 70, 61, 53, 45, 38, 32, 26, 20, 16, 12, 8, 5,
		3, 2, 1, 0, 1, 2, 3, 5, 8, 12, 16, 20, 26, 32, 38, 45, 53, 61, 70, 80, 90, 101,
		112, 124, 136, 150, 163, 177, 192, 208, 224, 240, 257, 275, 293, 312, 331, 350,
		371, 391, 413, 434, 457, 479, 503, 526, 550, 575, 600, 626, 651, 678, 705, 732,
		759, 787, 816, 844, 873, 903, 933, 963, 993, 1024, 1055, 1086, 1118, 1150, 1182,
		1215, 1248, 1281, 1314, 1347, 1381, 1415, 1449, 1483, 1518, 1552, 1587, 1622,
		1657, 1692, 1727, 1763, 1798, 1834, 1869, 1905, 1940, 1976, 2012};
		
uint16_t dac_buffer_lr[DAC_BUFFER_SIZE*2];

/*
 * Turn on the orange LED once DMA ends the transmission.
 */
static void daccb(DACDriver *dacp, const dacsample_t * samples, size_t pos) {
  (void)dacp;
  (void)pos;
  palTogglePad(GPIOD, GPIOD_LED3); // Orange
	
  /* samples is at the beginning or half of the buffer depending on which interrupt is generated
       we can use it in both cases.	*/
  dmaStartMemCopy(STM32_DMA2_STREAM7, 
    STM32_DMA_CR_PL(0) | STM32_DMA_CR_PSIZE_WORD | STM32_DMA_CR_MSIZE_WORD,
    dac_buffer, samples, sizeof(dac_buffer));

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
  48000, /* Multiply the buffer size to the desired frequency in Hz */
  DAC_DHRM_12BIT_RIGHT_DUAL, /* data holding register mode */
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
