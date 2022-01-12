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


bool hwInit(void);


#ifdef __cplusplus
}
#endif

#endif