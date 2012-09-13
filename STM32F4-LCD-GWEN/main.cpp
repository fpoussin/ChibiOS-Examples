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
	#include "chprintf.h"
	#include "shell.h"
	#include "shellconfig.h"
}

#include "Gwen/Input/ChibiGFX.h"
#include "Gwen/Renderers/ChibiGFX.h"
#include "Gwen/Skins/Light.h"
#include "gui.h"

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

  chprintf((BaseSequentialStream *)&SD2, "LCD Thread starting...\r\n");

    // Set the renderer to use ChibiOS/GFX and the light skin
	Renderer::ChibiGFX pRenderer = Renderer::ChibiGFX();
	Skin::Light skin;
	skin.SetRender( &pRenderer );

	const uint16_t width = pRenderer.getWidth();
	const uint16_t height = pRenderer.getHeight();

	// This is the canvas that will contain our controls
	Controls::Canvas pCanvas = Controls::Canvas( &skin );
	pCanvas.SetSize( width-1, height-1 );
	pCanvas.SetDrawBackground( true );
	pCanvas.SetBackgroundColor( Color( 0xBB, 0xBB, 0xBB, 0xFF ) );

	// This is where all our test controls are. We put it in the canvas.
	testControl tc(&pCanvas);
	tc.SetBounds( 0, 0, width-1, height-1 );

	// This is what handles the inputs such as touch screen and keys.
	Input::ChibiGFX GwenInput;
	GwenInput.Initialize( &pCanvas );

	// We map the discovery board's user button to the tab key
	GwenInput.AddKey(GPIOA, GPIOA_BUTTON, Input::ChibiGFX::KB_TAB);

	bool touch = false, prevtouch = false;
	while (TRUE) {

	// Only redraw if needed
	  if (pCanvas.NeedsRedraw()) {
		  pCanvas.RenderCanvas();
	  }
	  prevtouch = touch;
	  touch = GwenInput.Touched();

	  if (touch) { // If touch pressed, just process it and continue loop
		  GwenInput.ProcessTouch(touch);
		  chThdSleepMilliseconds(10);
		  continue; // skip the rest of the loop
	  }
	  else if (prevtouch != touch) // If touch state changed
		  GwenInput.ProcessTouch(touch);

	  GwenInput.ProcessKeys();

	  chThdSleepMilliseconds(10);
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

  Thread *shelltp = NULL;
  halInit();
  chSysInit();

  /*
   * SPI2 I/O pins setup.
   */
  palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);       /* New SCK.     */
  palSetPadMode(GPIOB, 14, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);       /* New MISO.    */
  palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST);       /* New MOSI.    */
  palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST);       /* New CS.      */
  palSetPad(GPIOB, 12);

  /*
    * Activates the serial driver 2 using the driver default configuration.
    * PA2(TX) and PA3(RX) are routed to USART2.
    */
  sdStart(&SD2, NULL);
  palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));
  shellInit();

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);
  while (TRUE) {
    if (!shelltp)
      shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
    else if (chThdTerminated(shelltp)) {
      chThdRelease(shelltp);    /* Recovers memory of the previous shell.   */
      shelltp = NULL;           /* Triggers spawning of a new shell.        */
    }
    chThdSleepMilliseconds(25);
  }
}
