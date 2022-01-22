#include "cli_run.h"


void cliRunInfo(cli_args_t *args);
void cliRunUart(cli_args_t *args);

void cliRunInit(void)
{
  cliAdd("info", cliRunInfo);  
  cliAdd("uart", cliRunUart);  
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