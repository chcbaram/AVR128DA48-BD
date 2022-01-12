#include "hw.h"


bool hwInit(void)
{
  bspInit();

  cliInit();
  ledInit();
  infoInit();
  uartInit();
  buttonInit();

  return true;
}