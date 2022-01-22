/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"


enum
{
  MODE_CLI,
  MODE_DOWN
};


typedef struct
{
  uint8_t mode;
} arg_option_t;



arg_option_t arg_option;


void apShowHelp(void);
bool apGetOption(int argc, char *argv[]);
void apCliMode(void);
void apDownMode(void);



void apInit(void)
{
  logPrintf("avr128da-ld V220122R1\n\n");

  arg_option.mode = MODE_DOWN;

  cliOpen(_DEF_UART1, 115200);

  cliRunInit();
}

void apMain(int argc, char *argv[])
{
  bool ret;

  ret = apGetOption(argc, argv);
  if (ret != true)
  {
    apExit();
    return;
  }

  if (arg_option.mode == MODE_CLI)
    apCliMode();
  else
    apDownMode();

  apExit();
}

bool apGetOption(int argc, char *argv[])
{
  int opt;
  
  while((opt = getopt(argc, argv, "hc")) != -1)
  {
    switch(opt)
    {
      case 'h':
        apShowHelp();
        break;

      case 'c':
        arg_option.mode = MODE_CLI;
        break;

      case '?':
        logPrintf("Unknown\n");
        break;
    }
  }

  return true;
}

void apShowHelp(void)
{
  logPrintf("avr128da-ld \n");
  logPrintf("            -h : help\n");
  logPrintf("            -c : cli mode\n");
  logPrintf("            -p com1  : com port\n");
  logPrintf("            -b 115200: baud\n");
  logPrintf("            -f fw.bin: firmware\n");
  logPrintf("            -r       : run firmware\n");
}

void apCliMode(void)
{
  logPrintf("Cli Mode Start...\n");

  while(1)
  {
    cliMain();
  }
}

void apDownMode(void)
{
  logPrintf("Down Mode Not Support\n");
}

void apExit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uartClose(i);
  }
  printf("\nexit\n");
  exit(0);
}

