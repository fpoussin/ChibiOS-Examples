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
#include "math.h"

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

static WORKING_AREA(waThread2, 4096);
__attribute__ ((__noreturn__))
static msg_t Thread2(void *arg)  {

  (void)arg;
  chRegSetThreadName("lcd");

  /* Reset */
  gfxInit();
  gdispClear(Black);

  //uint16_t width = gdispGetWidth();
  //uint16_t height = gdispGetHeight();

  ftfont_t ubuntu = gftOpenFont("Ubuntu mono");
  const char *msg = "ChibiOS/GFX with FreeType";

  while (TRUE) {

    gdispClear(Black);

    gftSetFontSize(ubuntu, 25);
    gftDrawString(100, 100, msg, ubuntu, SkyBlue);

    chThdSleepMilliseconds(1000);

    gdispClear(White);
    //gftSetFontSize(ubuntu, 10);
    gftDrawString(100, 100, msg, ubuntu, Red);

    chThdSleepMilliseconds(1000);
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
