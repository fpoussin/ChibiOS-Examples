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

extern "C" {
	#include "ch.h"
	#include "hal.h"
	#include "stdlib.h"
}
#include "Gwen/Renderers/ChibiGFX.h"
#include "Gwen/Skins/Simple.h"
#include "Gwen/Controls/Canvas.h"
#include "Gwen/Controls/Button.h"

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

  Gwen::Renderer::ChibiGFX* pRenderer = new Gwen::Renderer::ChibiGFX();
  Gwen::Skin::Simple skin;
  skin.SetRender( pRenderer );

  uint16_t width = pRenderer->getWidth();
  uint16_t height = pRenderer->getHeight();
  Gwen::Controls::Canvas* pCanvas = new Gwen::Controls::Canvas( &skin );
  pCanvas->SetSize( width, height );

  Gwen::Controls::Button* pButton = new Gwen::Controls::Button( pCanvas );
  pButton->SetBounds( 10, 10, 200, 100 );
  pButton->SetText( "My First Button" );

  while (TRUE) {
	  pCanvas->RenderCanvas();
	  chThdSleepMilliseconds(50);
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
   * Creates the example thread.
   */
//  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
