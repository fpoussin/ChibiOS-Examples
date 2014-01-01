#include "comm.h"
#include "common.h"

uint8_t checksum(uint8_t *data, uint8_t length)
{
    uint8_t i;
    uint8_t sum = 0;

    for (i = 0; i < length; i++)
      sum += data[i];

    return sum;
}

bool_t read_cmd(BaseChannel *chn)
{
  cmd_header_t header;
  uint8_t data_buf[DATA_BUF_SIZE];

  if ( !chnReadTimeout(chn, (uint8_t *)&header, 4, MS2ST(10) ) )
  {
    return false;
  }

  // Decode header
  if (header.magic1 != MAGIC1 || header.magic2 != MAGIC2 || !(header.type & CMD_MASK) || header.len < 5 )
  {
    return false;
  }

  if ( chnReadTimeout(chn, data_buf, header.len-4, MS2ST(100) ) < (header.len-4) )
  {
    return false;
  }

  uint8_t cs1 = checksum((uint8_t *)&header, sizeof(header)) + checksum(data_buf, header.len-4);
  uint8_t cs2 = data_buf[header.len-4];

  if (cs1 != cs2)
  {
    return false;
  }

  bool_t status = false;
  switch (header.type)
  {
    case CMD_ERASE:
      status = eraseFlash(leToInt(data_buf));
      break;

    case CMD_WRITE:
      status = writeHandler(data_buf, header.len-5);
      break;

    default:
      break;
  }


  return status;
}


bool_t writeHandler(uint8_t* buf, uint8_t len) {

  if (len % 4) {
    return false;
  }

  uint32_t address = leToInt(buf);
  uint32_t *data_buf = (uint32_t*)&buf[4];

  return writeFlash(address, data_buf, len/4);
}
