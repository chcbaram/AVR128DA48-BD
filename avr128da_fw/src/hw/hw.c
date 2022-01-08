#include "hw.h"





bool hwInit(void)
{
  bspInit();

  cliInit();
  ledInit();
  uartInit();

  return true;
}