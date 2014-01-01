#include "ch.h"
#include "hal.h"
#include "stm32f30x_flash.h"

#define MAGIC1 0xAF
#define MAGIC2 0xEB

#define DATA_MASK 0x40
#define CMD_MASK 0x80
#define CMD_ERASE 0x01
#define CMD_READ 0x02
#define CMD_WRITE 0x03


#define PKT_REQ_INFO 0x10
#define PKT_REQ_DATA 0x11

#define DATA_BUF_SIZE 256

typedef struct {
  uint8_t magic1;
  uint8_t magic2;
  uint8_t type;
  uint8_t len;
} cmd_header_t;


bool_t read_cmd(BaseChannel *chn);
bool_t writeHandler(uint8_t* buf, uint8_t len);

