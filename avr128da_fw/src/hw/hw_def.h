#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


#include "def.h"
#include "bsp.h"


#define _DEF_FIRMWATRE_VERSION    "V220201R1"
#define _DEF_BOARD_NAME           "AVR128DA48"



#define _USE_HW_INFO                
#define _USE_HW_RESET
#define _USE_HW_FLASH


#define _USE_HW_LED
#define      HW_LED_MAX_CH          1

#define _USE_HW_UART
#define      HW_UART_MAX_CH         2

#define _USE_HW_CLI
#define      HW_CLI_CMD_LIST_MAX    16
#define      HW_CLI_CMD_NAME_MAX    14
#define      HW_CLI_LINE_HIS_MAX    4
#define      HW_CLI_LINE_BUF_MAX    48

#define _USE_HW_BUTTON
#define      HW_BUTTON_MAX_CH       1
#define      HW_BUTTON_OBJ_USE      1

#define _USE_HW_GPIO
#define      HW_GPIO_MAX_CH         3

#define _USE_HW_SPI     
#define      HW_SPI_MAX_CH          2

#define _USE_HW_LCD
#define      HW_LCD_WIDTH           128
#define      HW_LCD_HEIGHT          64
#define _USE_HW_SSD1306
#define      HW_SSD1306_SPI   

#define _USE_HW_DXL
#define      HW_DXL_PACKET_BUF_MAX  512
#define      HW_DXL_DEVICE_CNT_MAX  16

#define _USE_HW_SW_TIMER
#define      HW_SW_TIMER_MAX_CH     4

#define _USE_HW_MCP2515
#define      HW_MCP2515_MAX_CH      1


#define FLASH_ADDR_BOOT             0x0000
#define FLASH_ADDR_BOOT_VER         0x00EC

#define FLASH_ADDR_FW               0x4000
#define FLASH_ADDR_FW_VER           0x40EC

#define _PIN_GPIO_LCD_DC            0
#define _PIN_GPIO_LCD_CS            1

#endif