#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#include "led.h"
#include "uart.h"
#include "cli.h"
#include "info.h"
#include "button.h"
#include "reset.h"
#include "flash.h"
#include "gpio.h"
#include "spi.h"
#include "lcd.h"
#include "dxl.h"
#include "sw_timer.h"


bool hwInit(void);

void jumpToApp(void);
void jumpToBoot(void);


#ifdef __cplusplus
}
#endif

#endif