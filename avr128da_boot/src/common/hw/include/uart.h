#ifndef SRC_UART_H_
#define SRC_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_UART

#define UART_MAX_CH     HW_UART_MAX_CH



bool     uartInit(void);
bool     uartDeInit(void);
bool     uartOpen(uint8_t ch, uint32_t baud);
bool     uartIsOpen(uint8_t ch);
bool     uartClose(uint8_t ch);
uint32_t uartAvailable(uint8_t ch);
void     uartFlush(uint8_t ch);
void     uartPutch(uint8_t ch, uint8_t data);
uint8_t  uartGetch(uint8_t ch);
int32_t  uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length);
uint8_t  uartRead(uint8_t ch);
int32_t  uartPrintf(uint8_t ch, const char *fmt, ...);
int32_t  uartPrintf_P(uint8_t ch, const char *fmt, ...);
uint32_t uartGetBaud(uint8_t ch);

#endif

#ifdef __cplusplus
}
#endif

#endif