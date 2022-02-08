#ifndef BUZZER_H_
#define BUZZER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_BUZZER



bool buzzerInit(void);
void buzzerOn(uint16_t freq_hz, uint16_t time_ms);
void buzzerOff(void);

#endif


#ifdef __cplusplus
}
#endif


#endif