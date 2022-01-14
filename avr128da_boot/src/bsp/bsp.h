#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>
#include <avr/fuse.h>
#include <util/delay.h>


bool bspInit(void);
bool bspDeInit(void);

void delay(uint32_t time_ms);
uint32_t millis(void);


void logPrintf(const char *fmt, ...);
void logPrintf_P(const char *fmt, ...);


/* Jump in program memory at a specific address */
extern void pgm_jmp_far(uint32_t addrr);

/* Write a word in program memory at a specific address*/
extern void pgm_word_write(uint32_t addrr, uint16_t data);


#ifdef __cplusplus
}
#endif

#endif