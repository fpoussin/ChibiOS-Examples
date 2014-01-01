#include "common.h"

typedef void (*FuncPtr)(void);

void jumpToUser(uint32_t usrAddr) {
    typedef void (*funcPtr)(void);

    uint32_t jumpAddr = *(volatile uint32_t *)(usrAddr + 0x04); /* reset ptr in vector table */
    funcPtr usrMain = (funcPtr) jumpAddr;

    __set_MSP(*(volatile uint32_t *) usrAddr);             /* set the users stack ptr */

    usrMain();                                /* go! */
}

bool_t eraseFlash(uint32_t len) {

  uint16_t i;
  const uint32_t from = USER_APP_ADDR;
  const uint32_t to = (USER_APP_ADDR+len);
  const uint16_t pages = (to - from) / FLASH_PAGE_SIZE;

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

  for (i=0; i <= pages; i++) {

    if (FLASH_ErasePage(from + (FLASH_PAGE_SIZE * i))!= FLASH_COMPLETE) {

      FLASH_Lock();
      return false;
    }
  }

  FLASH_Lock();
  return true;
}

bool_t writeFlash(uint32_t addr, uint32_t *buf, uint32_t len) {

  uint16_t i;

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

  for (i=0; i < len; i++) {

    if (FLASH_ProgramWord(addr+i, buf[i])!= FLASH_COMPLETE) {

      FLASH_Lock();
      return false;
    }
  }

  FLASH_Lock();
  return true;
}

uint32_t leToInt(uint8_t *ptr) {

  return ((uint32_t)ptr[3] << 24) |
      ((uint32_t)ptr[2] << 16) |
      ((uint32_t)ptr[1] << 8) |
      (uint32_t)ptr[0];
}

uint32_t beToInt(uint8_t *ptr) {

  return ((uint32_t)ptr[0] << 24) |
      ((uint32_t)ptr[1] << 16) |
      ((uint32_t)ptr[2] << 8) |
      (uint32_t)ptr[3];
}


