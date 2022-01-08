#include "hw.h"





bool hwInit(void)
{
  bspInit();

  ledInit();
  uartInit();

  return true;
}