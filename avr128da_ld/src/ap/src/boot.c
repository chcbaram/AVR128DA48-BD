#include "boot.h"




#define BOOT_CMD_READ_BOOT_VERSION      0x00
#define BOOT_CMD_READ_BOOT_NAME         0x01
#define BOOT_CMD_READ_FIRM_VERSION      0x02
#define BOOT_CMD_READ_FIRM_NAME         0x03
#define BOOT_CMD_FLASH_ERASE            0x04
#define BOOT_CMD_FLASH_WRITE            0x05
#define BOOT_CMD_FLASH_READ             0x06
#define BOOT_CMD_JUMP_TO_FW             0x08



static cmd_t cmd_boot;






bool bootInit(uint8_t ch, char *port_name, uint32_t baud)
{
  bool ret;

  cmdInit(&cmd_boot);

  if (port_name != NULL)
  {
    uartSetPortName(ch, port_name);
  }

  // Jump To Bootloader
  //
  if (uartIsOpen(ch) == true)
  {
    uartClose(ch);  
  }

  uartOpen(ch, baud);
  uartPrintf(ch, "\r\nreset boot 0x01\r\n");  
  delay(10);
  uartClose(ch);
  delay(100);  

  ret = cmdOpen(&cmd_boot, ch, baud);

  return ret;
}

bool bootDeInit(uint8_t ch)
{
  bool ret;

  ret = uartClose(ch);

  return ret;
}


uint8_t bootCmdReadBootVersion(char *p_str)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;



  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_BOOT_VERSION, NULL, 0, 100) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->rx_packet;

    if (cmd_boot.error == CMD_OK)
    {
      for (int i=0; i<p_packet->length; i++)
      {
        p_str[i] = p_packet->data[i];
      }
      p_str[p_packet->length] = 0;
    }
  } 

  ret = p_cmd->error;

  return ret;
}


uint8_t bootCmdReadBootName(char *p_str)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;



  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_READ_BOOT_NAME, NULL, 0, 100) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->rx_packet;

    if (cmd_boot.error == CMD_OK)
    {
      for (int i=0; i<p_packet->length; i++)
      {
        p_str[i] = p_packet->data[i];
      }
      p_str[p_packet->length] = 0;
    }
  } 

  ret = p_cmd->error;

  return ret;
}

//void bootCmdReadFirmVersion(cmd_t *p_cmd);
//void bootCmdReadFirmName(cmd_t *p_cmd);

uint8_t bootCmdFlashErase(uint32_t addr, uint32_t length, uint32_t timeout)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t tx_buf[8];


  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;

  cmdSendCmdRxResp(p_cmd, BOOT_CMD_FLASH_ERASE, tx_buf, 8, timeout);

  ret = p_cmd->error;

  return ret;
}

uint8_t bootCmdFlashWrite(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t *tx_buf;


  tx_buf = p_cmd->tx_packet.data;

  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;

  for (int i=0; i<length; i++)
  {
    tx_buf[8+i] = p_data[i];  
  }

  cmdSendCmdRxResp(p_cmd, BOOT_CMD_FLASH_WRITE, tx_buf, 8+length, timeout);

  ret = p_cmd->error;

  return ret;  
}

uint8_t bootCmdFlashRead(uint32_t addr, uint8_t *p_data, uint32_t length, uint32_t timeout)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;
  uint8_t *tx_buf;


  tx_buf = p_cmd->tx_packet.data;

  tx_buf[0] = (addr >>  0) & 0xFF;
  tx_buf[1] = (addr >>  8) & 0xFF;
  tx_buf[2] = (addr >> 16) & 0xFF;
  tx_buf[3] = (addr >> 24) & 0xFF;

  tx_buf[4] = (length >>  0) & 0xFF;
  tx_buf[5] = (length >>  8) & 0xFF;
  tx_buf[6] = (length >> 16) & 0xFF;
  tx_buf[7] = (length >> 24) & 0xFF;


  if (cmdSendCmdRxResp(p_cmd, BOOT_CMD_FLASH_READ, tx_buf, 8, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->rx_packet;

    if (p_cmd->error == CMD_OK)
    {
      for (int i=0; i<p_packet->length; i++)
      {
        p_data[i] = p_packet->data[i];
      }  
    }
  }

  ret = p_cmd->error;

  return ret;  
}

uint8_t bootCmdJumpToFw(void)
{
  uint8_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd_boot;


  cmdSendCmdRxResp(p_cmd, BOOT_CMD_JUMP_TO_FW, NULL, 0, 100);

  ret = p_cmd->error;

  return ret;  
}