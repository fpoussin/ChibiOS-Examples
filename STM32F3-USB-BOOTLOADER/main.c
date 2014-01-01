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

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "common.h"
#include "usb_config.h"
#include "stm32f30x_iwdg.h"
#include "comm.h"

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    systime_t time = USBD1.state == USB_ACTIVE ? 250 : 500;
    palClearPad(GPIOE, GPIOE_LED3_RED);
    chThdSleepMilliseconds(time);
    palSetPad(GPIOE, GPIOE_LED3_RED);
    chThdSleepMilliseconds(time);
  }
}

/*
 * USB Bulk thread, times are in milliseconds.
 */
static WORKING_AREA(waThread2, 256);
static msg_t Thread2(void *arg) {

  EventListener el1;
  flagsmask_t flags;
  (void)arg;
  chRegSetThreadName("USB Bulk");
  chEvtRegisterMask(chnGetEventSource(&BDU1), &el1, CHN_INPUT_AVAILABLE);

  while(BDU1.state != BDU_READY) chThdSleepMilliseconds(100);

  while (TRUE) {

    chEvtWaitOneTimeout(EVENT_MASK(1), MS2ST(10));
    flags = chEvtGetAndClearFlags(&el1);

    if (flags & CHN_INPUT_AVAILABLE) {

      read_cmd((BaseChannel *)&BDU1);
    }
  }
}

/*
 * Application entry point.
 */
int main(void) {

  bool_t sftrst = false;
  bool_t iwdgrst = false;

  /*!< Independent Watchdog reset flag */
  if (RCC->CSR & RCC_CSR_IWDGRSTF) {
    /* User App did not start properly */

    iwdgrst = true;
  }

  /*!< Software Reset flag */
  else if (RCC->CSR & RCC_CSR_SFTRSTF) {
    /* Bootloader called by user app */

    sftrst = true;
  }

  else {
/*
    chSysLockFromIsr();

    usbDisconnectBus(&USBD1);
    usbStop(&USBD1);

    chSysDisable();
*/
    /* Setup IWDG in case the target application does not load */

    const uint32_t LsiFreq = 42000;
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms * IWDG counter clock period
                            = 250ms * (LSI/32)
                            = 0.25s * (LsiFreq/32)
                            = LsiFreq / (32 * 4)
                            = LsiFreq / 128
     */
    IWDG_SetReload(LsiFreq/128);

    IWDG_Enable();

    jumpToUser(USER_APP_ADDR);
  }

  /*!< Remove reset flags */
  RCC->CSR |= RCC_CSR_RMVF;

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  if (iwdgrst == true) {

    palSetPad(GPIOE, GPIOE_LED5_ORANGE);
  }

  if (sftrst == true) {

    palSetPad(GPIOE, GPIOE_LED7_GREEN);
  }

  /*
   * Initializes a Bulk USB driver.
   */
  bduObjectInit(&BDU1);
  bduStart(&BDU1, &bulkusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  //chThdSleepMilliseconds(1500);
  usbStart(&USBD1, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /*
   * Creates the blinker and bulk threads.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(waThread2, sizeof(waThread2), NORMALPRIO, Thread2, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    chThdSleepMilliseconds(1000);
  }
}
