#ifndef SRC_COMMON_DEF_H_
#define SRC_COMMON_DEF_H_


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


#define _DEF_LED1             0
#define _DEF_LED2             1
#define _DEF_LED3             2
#define _DEF_LED4             3

#define _DEF_UART1            0
#define _DEF_UART2            1
#define _DEF_UART3            2
#define _DEF_UART4            3

#define _DEF_LOW              0
#define _DEF_HIGH             1

#define _DEF_BUTTON1          0
#define _DEF_BUTTON2          1
#define _DEF_BUTTON3          2
#define _DEF_BUTTON4          3

#define _DEF_SPI1             0
#define _DEF_SPI2             1
#define _DEF_SPI3             2
#define _DEF_SPI4             3

#define _DEF_INPUT            0
#define _DEF_INPUT_PULLUP     1
#define _DEF_INPUT_PULLDOWN   2
#define _DEF_OUTPUT           3
#define _DEF_OUTPUT_PULLUP    4
#define _DEF_OUTPUT_PULLDOWN  5

#define _DEF_DXL1             0
#define _DEF_DXL2             1
#define _DEF_DXL3             2
#define _DEF_DXL4             3

#define _DEF_CAN1             0
#define _DEF_CAN2             1
#define _DEF_CAN3             2
#define _DEF_CAN4             3

#define _DEF_CAN_100K         0
#define _DEF_CAN_125K         1
#define _DEF_CAN_250K         2
#define _DEF_CAN_500K         3
#define _DEF_CAN_1000K        4

#define _DEF_CAN_STD          0
#define _DEF_CAN_EXT          1


#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif


#ifndef map
#define map(value, in_min, in_max, out_min, out_max) ((value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#endif


#define VERSION_MAGIC_NUMBER      0x56455220    // "VER "


typedef struct 
{
  uint32_t magic_number;
  char version_str[32];
  char name_str[32];
} firm_ver_t;

#endif