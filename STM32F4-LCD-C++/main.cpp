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
#include "test.h"
#include "stdlib.h"

#include "gfx.h"

#include <stdio.h>

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

static WORKING_AREA(waThreadLoad1, 128);
__attribute__ ((__noreturn__))
static msg_t ThreadLoad1(void *arg) {

  (void)arg;
  chRegSetThreadName("load1");
  uint32_t i;
  while (TRUE) i++;
}

static WORKING_AREA(waThreadLoad2, 128);
__attribute__ ((__noreturn__))
static msg_t ThreadLoad2(void *arg) {

  (void)arg;
  chRegSetThreadName("load2");
  uint32_t i;
  while (TRUE) i++;
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
	srand(halGetCounterValue());
	GEventMouse	ev;
	ginputGetMouse(0);
	gdispClear(Black);

	uint16_t width = gdispGetWidth();
	uint16_t height = gdispGetHeight();

	const font_t font1 = gdispOpenFont("UI2 Double");

	uint32_t i=0, pixels;
	color_t random_color;
	uint16_t rx, ry, rcx, rcy;
	gdispDrawString(50, height/2, "Draw pixels or quit using the cross", font1, Red);
	gdispDrawString(0, 0, "X", font1, White);
	 while (TRUE) {
		 if (ev.current_buttons & GINPUT_MOUSE_BTN_LEFT) {

			ginputGetMouseStatus(0, &ev);

			if (ev.x < 20 && ev.y < 20) break;
			gdispDrawPixel(ev.x, ev.y, rand() % 65535);
			gdispDrawString(0, 0, "X", font1, White);
		 }
		 chThdSleepMilliseconds(5);
	 }
	 gdispClear(Black);
  while (TRUE) {

	  gdispFillArea(10, 10, width-20, height-20, Grey);
	  gdispFillArea(30, 30, 300, 150, Red);
	  gdispFillArea(50, 50, 200, 100, Blue);
	  gdispFillArea(80, 80, 150, 50, Yellow);
	  gdispFillCircle(width/2, height/2, 50, White);

	  const char *msg = "ChibiOS/GFX on SSD1963";
	  gdispDrawString(width-gdispGetStringWidth(msg, font1)-3, height-24, msg, font1, White);

	  chThdSleepMilliseconds(1000);


	  pixels = 0;
	  gdispClear(Black);
	  gdispDrawString(50, height/2, "Doing 5000 random rectangles", font1, White);
	  chThdSleepMilliseconds(2000);
	  uint32_t start = halGetCounterValue();
	  for (i = 0; i < 5000; i++) {
		  random_color = (rand() % 65535);
		  rx = (rand() % (width-10));
		  ry = (rand() % (height-10));
		  rcx = (rand() % ((width-rx)-10))+10;
		  rcy = (rand() % ((height-ry)-10))+10;

		  gdispFillArea(rx, ry, rcx, rcy, random_color);
		  pixels += (rcx+1)*(rcy+1);
	  }
	  uint32_t ms = (halGetCounterValue()-start) / (halGetCounterFrequency()/1000);
	  uint32_t pps = (float)pixels/((float)ms/1000.0f);

	  char pps_str[20];
	  snprintf(pps_str, sizeof(pps_str), "%lu Pixels/s", pps);

	  gdispClear(Black);
	  gdispDrawString(100, height/2, pps_str, font1, White);
	  chThdSleepMilliseconds(3000);
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
   * SPI2 I/O pins setup.
   */
  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);       /* New SCK.     */
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);       /* New MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                           PAL_STM32_OSPEED_HIGHEST);       /* New MOSI.    */
  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL |
                           PAL_STM32_OSPEED_HIGHEST);       /* New CS.      */
  palSetPad(GPIOB, 12);

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  //chThdCreateStatic(waThreadLoad1, sizeof(waThreadLoad1), NORMALPRIO, ThreadLoad1, NULL);
  //chThdCreateStatic(waThreadLoad2, sizeof(waThreadLoad2), NORMALPRIO, ThreadLoad2, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
