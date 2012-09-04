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
#include "test.h"
#include "stdlib.h"

#include <string>
#include <sstream>

#include "gdisp.h"
#include "console.h"

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
//  static GLCDConsole CON1;
	gdispInit();
//	gdispSetOrientation(landscape);
	gdispClear(Black);

	uint16_t width = gdispGetWidth();
	uint16_t height = gdispGetHeight();

	uint32_t pixels;
	uint32_t i;
	color_t random_color;
	uint16_t rx, ry, rcx, rcy;

	srand(halGetCounterValue());
  while (TRUE) {

//		lcdConsoleInit(&CON1, 0, 0, gdispGetWidth(), gdispGetHeight(), &fontLarger, Black, White);
//		TestThread(&CON1);

	  gdispFillArea(10, 10, width-20, height-20, Grey);
	  gdispFillArea(30, 30, 300, 150, Red);
	  gdispFillArea(50, 50, 200, 100, Blue);
	  gdispFillArea(80, 80, 150, 50, Yellow);
	  gdispFillCircle(width/2, height/2, 50, White);

	  const char *msg = "ChibiOS/GFX on SSD1963";
	  gdispDrawString(width-gdispGetStringWidth(msg, &fontUI2Double)-3, height-24, msg, &fontUI2Double, White);

	  chThdSleepMilliseconds(1000);


	  pixels = 0;
	  gdispClear(Black);
	  gdispDrawString(50, height/2, "Doing 5000 random rectangles", &fontUI2Double, White);
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
	  uint32_t ms = (halGetCounterValue()-start) / 168000;
	  uint32_t pps = (float)pixels/((float)ms/1000.0f);

	  std::stringstream pps_ss;
	  pps_ss << pps << " Pixels/s";

	  gdispClear(Black);
	  gdispDrawString(100, height/2, pps_ss.str().c_str(), &fontUI2Double, White);
	  chThdSleepMilliseconds(3000);
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
