#include "cli_run.h"


void cliRunInfo(cli_args_t *args);
void cliRunUart(cli_args_t *args);
void cliRunBoot(cli_args_t *args);



void cliRunInit(void)
{
  cliAdd("info", cliRunInfo);  
  cliAdd("uart", cliRunUart);  
  cliAdd("boot", cliRunBoot);  
}

void cliRunInfo(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 0)
  {
    logPrintf("This is info\n");
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("info\n");
  }
}

void cliRunUart(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      cliPrintf("Uart  : Opened\n");
      cliPrintf("Port  : %s\n", uartGetPortName(_USE_UART_CMD));
      cliPrintf("Baud  : %d bps\n", uartGetBaud(_USE_UART_CMD));
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
    }
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "list"))
  {
    char port_str[128];

    for (int i=0; i<100; i++)
    {
      snprintf(port_str, 128, "com%d", i);
      if (uartOpenPort(_USE_UART_CMD, port_str, 115200) == true)
      {
        cliPrintf("COM%d \t: Found\n", i);
        uartClose(_USE_UART_CMD);
      }
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "open"))
  {
    char *port_str;
    uint32_t port_baud;
    bool uart_ret;

    port_str = args->getStr(1);
    port_baud = args->getData(2);

    uart_ret = uartOpenPort(_USE_UART_CMD, port_str, port_baud);

    cliPrintf("Port : %s\n", port_str);
    cliPrintf("Baud : %d bps\n", port_baud);
    if (uart_ret)
      cliPrintf("Open : True\n");
    else
      cliPrintf("Open : False\n");

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    bool uart_ret = false;

    if (uartIsOpen(_USE_UART_CMD))
    {
      cliPrintf("Uart  : Opened\n");
      cliPrintf("Port  : %s\n", uartGetPortName(_USE_UART_CMD));
      cliPrintf("Baud  : %d bps\n", uartGetBaud(_USE_UART_CMD));
      uart_ret = true;
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");
    }

    while(cliKeepLoop() && uart_ret == true)
    {
      if (uartAvailable(_USE_UART_CMD) > 0)
      {
        uint8_t rx_data;

        rx_data = uartRead(_USE_UART_CMD);
        uartWrite(_USE_UART_CLI, &rx_data, 1);
      }  
    }

    ret = true;
  }


  if (ret == false)
  {
    cliPrintf("uart info\n");
    cliPrintf("uart list\n");
    cliPrintf("uart open comport baud\n");
    cliPrintf("uart test \n");
  }
}

void cliRunBoot(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1)
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t cmd;
      cmd_t cmd_boot;

      cmd = args->getData(0);

      cmdInit(&cmd_boot);
      cmdOpen(&cmd_boot, _USE_UART_CMD, uartGetBaud(_USE_UART_CMD));

      cliPrintf("cmd  : 0x%02X\n", cmd);
      if (cmdSendCmdRxResp(&cmd_boot, cmd, NULL, 0, 100) == true)
      {
        cmd_packet_t *p_packet = &cmd_boot.rx_packet;

        p_packet->data[p_packet->length] = 0;
        cliPrintf("resp : %s\n", p_packet->data);
      } 
      else
      {
        cliPrintf("not resp\n");
      } 
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_read"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t cmd;
      cmd_t cmd_boot;
      uint32_t addr;
      uint32_t length;
      uint8_t tx_buf[8];

      cmd = 0x06;
      addr = args->getData(1);
      length = args->getData(2);

      cmdInit(&cmd_boot);
      cmdOpen(&cmd_boot, _USE_UART_CMD, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash read 0x%X, %d\n", addr, length);

      tx_buf[0] = (addr >>  0) & 0xFF;
      tx_buf[1] = (addr >>  8) & 0xFF;
      tx_buf[2] = (addr >> 16) & 0xFF;
      tx_buf[3] = (addr >> 24) & 0xFF;

      tx_buf[4] = (length >>  0) & 0xFF;
      tx_buf[5] = (length >>  8) & 0xFF;
      tx_buf[6] = (length >> 16) & 0xFF;
      tx_buf[7] = (length >> 24) & 0xFF;

      if (cmdSendCmdRxResp(&cmd_boot, cmd, tx_buf, 8, 100) == true)
      {
        cmd_packet_t *p_packet = &cmd_boot.rx_packet;

        for (int i=0; i<p_packet->length; i++)
        {
          cliPrintf("resp : 0x%X : 0x%X\n", addr + i, p_packet->data[i]);
        }
      } 
      else
      {
        cliPrintf("not resp\n");
      } 
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_write"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t cmd;
      cmd_t cmd_boot;
      uint32_t addr;
      uint32_t length;
      uint32_t data;
      uint8_t tx_buf[8+4];

      cmd = 0x05;
      addr = args->getData(1);
      data = args->getData(2);
      length = 4;

      cmdInit(&cmd_boot);
      cmdOpen(&cmd_boot, _USE_UART_CMD, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash write 0x%X, 0x%X\n", addr, data);

      tx_buf[0] = (addr >>  0) & 0xFF;
      tx_buf[1] = (addr >>  8) & 0xFF;
      tx_buf[2] = (addr >> 16) & 0xFF;
      tx_buf[3] = (addr >> 24) & 0xFF;

      tx_buf[4] = (length >>  0) & 0xFF;
      tx_buf[5] = (length >>  8) & 0xFF;
      tx_buf[6] = (length >> 16) & 0xFF;
      tx_buf[7] = (length >> 24) & 0xFF;

      tx_buf[8]  = (data >>  0) & 0xFF;
      tx_buf[9]  = (data >>  8) & 0xFF;
      tx_buf[10] = (data >> 16) & 0xFF;
      tx_buf[11] = (data >> 24) & 0xFF;

      if (cmdSendCmdRxResp(&cmd_boot, cmd, tx_buf, 12, 500) == true)
      {
        cliPrintf("error : %d \n", cmd_boot.rx_packet.error);
      } 
      else
      {
        cliPrintf("not resp\n");
      } 
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "flash_erase"))
  {
    if (uartIsOpen(_USE_UART_CMD))
    {
      uint8_t cmd;
      cmd_t cmd_boot;
      uint32_t addr;
      uint32_t length;
      uint8_t tx_buf[8];

      cmd = 0x04;
      addr = args->getData(1);
      length = args->getData(2);

      cmdInit(&cmd_boot);
      cmdOpen(&cmd_boot, _USE_UART_CMD, uartGetBaud(_USE_UART_CMD));

      cliPrintf("flash erase 0x%X, %d\n", addr, length);

      tx_buf[0] = (addr >>  0) & 0xFF;
      tx_buf[1] = (addr >>  8) & 0xFF;
      tx_buf[2] = (addr >> 16) & 0xFF;
      tx_buf[3] = (addr >> 24) & 0xFF;

      tx_buf[4] = (length >>  0) & 0xFF;
      tx_buf[5] = (length >>  8) & 0xFF;
      tx_buf[6] = (length >> 16) & 0xFF;
      tx_buf[7] = (length >> 24) & 0xFF;

      if (cmdSendCmdRxResp(&cmd_boot, cmd, tx_buf, 8, 500) == true)
      {
        cliPrintf("error : %d \n", cmd_boot.rx_packet.error);
      } 
      else
      {
        cliPrintf("not resp\n");
      } 
    }
    else
    {
      cliPrintf("Uart  : Closed\n");
      cliPrintf("Need to run uart open\n");      
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("boot cmd[0x00~0xFF] \n");
    cliPrintf("boot flash_read addr[0x4000~] length[~256]\n");
    cliPrintf("boot flash_write addr[0x4000~] data\n");
    cliPrintf("boot flash_erase addr[0x4000~] length\n");
  }
}