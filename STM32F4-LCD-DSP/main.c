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

#define FFT_SIZE 1024 // 4096-1024-256-64-16 lengths supported by DSP library
#define SIN_TABLE_SIZE (FFT_SIZE*2)
#define DAC_BUFFER_SIZE 360
#define TWO_PI (M_PI * 2.0f)
#define SAMPLING_RATE 67300.0

#define COLOR1 HTML2COLOR(0xB00000)
#define COLOR2 HTML2COLOR(0x0000B0)

#define ADC_GRP1_NUM_CHANNELS   1
#define ADC_GRP1_BUF_DEPTH      SIN_TABLE_SIZE

static adcsample_t ADCSamples[SIN_TABLE_SIZE];

q15_t samples[SIN_TABLE_SIZE];
q15_t testOutput[FFT_SIZE/2];
extern const float32_t testInput_q15_10khz[2048];

int uitoa(unsigned int value, char * buf, int max);

/*
 * DAC test buffer (sine wave)
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


/*
 * Turn on the orange LED once DMA ends the transmission.
 */
static void daccb(DACDriver *dacp) {
  (void)dacp;
  palTogglePad(GPIOD, GPIOD_LED3); // Orange
}

/*
 * Turn on the red LED if there are DMA errors.
 */
static void dacerrcb(DACDriver *dacp) {
  (void)dacp;
  palTogglePad(GPIOD, GPIOD_LED4); // Red
}

/*
 * DAC config, with callbacks.
 */
static const DACConfig daccfg1 = {
  DAC_MODE_CONTINUOUS,
  960*DAC_BUFFER_SIZE, /* Multiply the buffer size to the desired frequency in Hz */
  dac_buffer, /* Pointer to the first buffer */
  NULL, /* Pointer to the second buffer */
  DAC_BUFFER_SIZE, /* Buffers size */
  daccb, /* End of transfer callback */
  dacerrcb, /* Error callback */
  /* STM32 specific config starts here */
  DAC_DHRM_12BIT_RIGHT, /* data holding register mode */
  0 /* CR flags */
};

static const DACConfig daccfg2 = {
  DAC_MODE_CONTINUOUS,
  4800*DAC_BUFFER_SIZE, /* Multiply the buffer size to the desired frequency in Hz */
  dac_buffer, /* Pointer to the first buffer */
  NULL, /* Pointer to the second buffer */
  DAC_BUFFER_SIZE, /* Buffers size */
  daccb, /* End of transfer callback */
  dacerrcb, /* Error callback */
  /* STM32 specific config starts here */
  DAC_DHRM_12BIT_RIGHT, /* data holding register mode */
  0 /* CR flags */
};

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 1 channel, SW triggered.
 * Channels:    IN11.
 */
static const ADCConversionGroup adcgrpcfg1 = {
  TRUE,
  ADC_GRP1_NUM_CHANNELS,
  NULL,
  NULL,
  0,                        /* CR1 */
  ADC_CR2_SWSTART | ADC_CR2_ALIGN,          /* CR2 */ // Align results to left
  ADC_SMPR1_SMP_AN11(ADC_SAMPLE_144), // ADCCLK = 84/8 = 10.5Mhz. Sampling time 10.5/(144+12) = 67KHz
  0,                        /* SMPR2 */
  ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),
  0,
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN11)
};

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

	gfxInit();
	gdispClear(Black);

	const uint16_t width = gdispGetWidth();
	const uint16_t height = gdispGetHeight();

	const font_t font1 = gdispOpenFont("UI2 Double");

	uint32_t cnt, us;
	char time_str[8];
	char freq_str[8];

	/* ------------------------------------------------------------------
	* Global variables for FFT Bin Example
	* ------------------------------------------------------------------- */
	const uint32_t ifftFlag = 0;
	const uint32_t doBitReverse = 1;
	arm_cfft_radix4_instance_q15 S;
	q15_t maxValue = 0;
	uint32_t maxIndex = 0;
	/*float32_t phaseIncrement = 0.0;
	float32_t currentPhase = 0.0;
	float32_t magnitudeIncrement = 0.25;
	float32_t currentMagnitude = 0.0;*/

	gdispDrawBox(0, 0, width, 108, White);
	gdispDrawBox(0, 112, width, 108, White);

	gdispDrawString(0, height-48, "FFT+MAG Processing time in us", font1, White);
	gdispDrawString(0, height-24, "Peak Frequency in Hz", font1, White);

	/* Initialize the CFFT/CIFFT module */
	arm_cfft_radix4_init_q15(&S, FFT_SIZE, ifftFlag, doBitReverse);

	while (TRUE) {

		gdispFillArea(1, 1, width-2, 106, COLOR1);
		gdispFillArea(1, 113, width-2, 106, COLOR2);

		int32_t i, s;
/*
		currentMagnitude += magnitudeIncrement;
		//currentMagnitude = 50.0;
		phaseIncrement += 0.05;
		//phaseIncrement = TWO_PI/(SAMPLING_RATE/4800.0); // Last number is desired frequency
		if (phaseIncrement >= M_PI) phaseIncrement = -M_PI;
		if (currentMagnitude >= 1.0) currentMagnitude = -1.0;
		currentPhase = 0.0;
*/
		for (i=0 ; i < SIN_TABLE_SIZE ; i+=4) // Unroll loops
		{
			samples[i] = 0;
			samples[i+1] = 0;
			samples[i+2] = 0;
			samples[i+3] = 0;
		}/*
		for (i=0 ; i < FFT_SIZE ; i+=4)
		{
			samples[i*2] = currentMagnitude*32768.0*arm_sin_f32(currentPhase += phaseIncrement); // Float to Q15
			samples[(i*2)+2] = currentMagnitude*32768.0*arm_sin_f32(currentPhase += phaseIncrement); // Float to Q15
			samples[(i*2)+4] = currentMagnitude*32768.0*arm_sin_f32(currentPhase += phaseIncrement); // Float to Q15
			samples[(i*2)+6] = currentMagnitude*32768.0*arm_sin_f32(currentPhase += phaseIncrement); // Float to Q15
		}
*/

		//dacStartSend(&DACD1, 32, dac_buffer);
		for (i=0 ; i < FFT_SIZE ; i+=4)
		{
			samples[i*2] = ADCSamples[i]-32767;
			samples[(i*2)+2] = ADCSamples[i+1]-32767;
			samples[(i*2)+4] = ADCSamples[i+2]-32767;
			samples[(i*2)+6] = ADCSamples[i+3]-32767;
		}
		// Need to draw first graph (Sine wave) before running FFT as it changes the sample data.
		for (i=0 ; i < FFT_SIZE && i < width-2 ; i++) // Sine wave graph
		{
			s = (samples[i*2] / (32768/52)); // Q15 to int, then scaled to 32768 = 52px
			if (s > 52.0) s = 52.0;
			else if (s < -52.0) s = -52.0;
			gdispDrawLine(1+i, 54-s, 1+i, 54,  White);
			gdispDrawLine(1+i, 54-s, 1+i, 54,  White);
		}
		//gdispDrawLine(1, 54, width-2, 54, Grey);

		//arm_copy_f32(testInput_q15_10khz, samples, 2048);
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

		// Second graph (FFT)
		for (i=0 ; i < FFT_SIZE/2 && i < width-2 ; i++) // We only display the first half as the rest is useless.
		{
			s = testOutput[i+16] / (maxValue/106);
			if (s > 106.0) s = 106.0;
			gdispFillArea(i, 220-s, 1, s, White);
		}

		gdispFillArea(width-80, height-48, 80, 48, Black);
		gdispDrawString(width-gdispGetStringWidth(time_str, font1)-3, height-48, time_str, font1, White);
		gdispDrawString(width-gdispGetStringWidth(freq_str, font1)-3, height-24, freq_str, font1, White);

		chThdSleepMilliseconds(50);
	}
}

static WORKING_AREA(waThread1, 128);
__attribute__ ((__noreturn__))
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
   // palSetPad(GPIOD, GPIOD_LED3);       /* Orange.  */
    //chThdSleepMilliseconds(500);
    //palClearPad(GPIOD, GPIOD_LED3);     /* Orange.  */
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
   * Setting up analog inputs used by the demo.
   */
  palSetPadMode(GPIOC, 1, PAL_MODE_INPUT_ANALOG);

  /*
   * DAC pin setup.
   */
  palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG); /* Manual says it prevents parasitic consumption,
                                                  the mode is ignored when the DAC_CR_ENx is activated */
  palSetPadMode(GPIOA, 5, PAL_MODE_INPUT_ANALOG);
  /*
   * Starting the DAC driver threads.
   */
  dacStart(&DACD1, &daccfg1);
  dacStart(&DACD2, &daccfg2);

  /*
   * Activates the ADC1 driver and the temperature sensor.
   */
  adcStart(&ADCD1, NULL);
  adcSTM32EnableTSVREFE();

  /*
   * Sending the dac_buffer
   */
  dacStartSend(&DACD1);
  dacStartSend(&DACD2);

  /*
   * Starts an ADC continuous conversion.
   */
  adcStartConversion(&ADCD1, &adcgrpcfg1, ADCSamples, ADC_GRP1_BUF_DEPTH);

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
