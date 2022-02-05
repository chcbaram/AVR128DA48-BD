#ifndef SRC_COMMON_HW_INCLUDE_SWTIMER_H_
#define SRC_COMMON_HW_INCLUDE_SWTIMER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_SW_TIMER



#define SW_TIMER_MAX_CH        HW_SW_TIMER_MAX_CH


typedef enum 
{
  ONE_TIME  = 1,
  LOOP_TIME = 2,
} SwTimerMode_t;



bool swTimerInit(void);
void swTimerSet  (uint8_t ch, uint32_t timer_ms, SwTimerMode_t timer_mode, void (*func)(void *), void *arg);
void swTimerStart(uint8_t ch);
void swTimerStop (uint8_t ch);
void swTimerReset(uint8_t ch);
void swTimerISR(void);


uint8_t  swTimerGetHandle(void);
uint32_t swTimerGetCounter(void);

#endif

#ifdef __cplusplus
}
#endif




#endif