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

#include "gdisp.h"
#include "console.h"
#include "touchpad.h"

#include <string>
#include <stdio.h>

static const SPIConfig spicfg = {
    NULL,
    TP_CS_PORT,
    TP_CS,
    /* SPI_CR1_BR_2 |*/ SPI_CR1_BR_1 | SPI_CR1_BR_0,
};

TOUCHPADDriver TOUCHPADD1 = {
	&SPID2,
	&spicfg,
	TP_IRQ_PORT,
	TP_IRQ,
	TRUE
};

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
	srand(halGetCounterValue());
	tpInit(&TOUCHPADD1);
	tpCalibrate();
//	gdispSetOrientation(landscape);
	gdispClear(Black);

	uint16_t width = gdispGetWidth();
	uint16_t height = gdispGetHeight();

	uint32_t i=0, pixels;
	color_t random_color;
	uint16_t rx, ry, rcx, rcy;
	volatile uint16_t x, y;
	gdispDrawString(50, height/2, "Draw pixels or quit using the cross", &fontUI2Double, Red);
	gdispDrawString(0, 0, "X", &fontUI2Double, White);
	 while (TRUE) {
		 if (tpIRQ()) {
			 // This is a dirty hack to get the position right...
			x = ((float)width-((float)tpReadX()*2.0f))*TP_W_FIX;
			y = ((float)tpReadY()/2.0f)*TP_H_FIX;

			if (x < 20 && y < 20) break;
			gdispDrawPixel(x, y, rand() % 65535);
			gdispDrawString(0, 0, "X", &fontUI2Double, White);
		 }
		 chThdSleepMilliseconds(5);
	 }
	 gdispClear(Black);
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
	  uint32_t ms = (halGetCounterValue()-start) / (halGetCounterFrequency()/1000);
	  uint32_t pps = (float)pixels/((float)ms/1000.0f);

	  char buf[20];
	  snprintf(buf, sizeof(buf), "%u Pixels/s", pps);
	  std::string pps_str = std::string(buf);

	  gdispClear(Black);
	  gdispDrawString(100, height/2, pps_str.c_str(), &fontUI2Double, White);
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
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
