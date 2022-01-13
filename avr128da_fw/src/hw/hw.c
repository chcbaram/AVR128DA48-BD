#include "hw.h"


bool hwInit(void)
{
  bspInit();
  
  #ifdef _USE_HW_CLI
  cliInit();  
  #endif
  ledInit();
  infoInit();
  uartInit();
  buttonInit();
  resetInit();
  flashInit();

  return true;
}