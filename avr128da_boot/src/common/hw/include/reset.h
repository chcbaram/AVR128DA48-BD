#ifndef SRC_COMMON_HW_INCLUDE_RESET_H_
#define SRC_COMMON_HW_INCLUDE_RESET_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"


#ifdef _USE_HW_RESET



enum ResetMode
{
  RESET_MODE_RUN_BOOT = (1<<0),
  RESET_MODE_RUN_APP  = (1<<1),
};

enum ResetSrc
{
  RESET_FROM_POWER = (1<<0),
  RESET_FROM_BOD   = (1<<1),
  RESET_FROM_PIN   = (1<<2),
  RESET_FROM_WDG   = (1<<3),
  RESET_FROM_SOFT  = (1<<4),
  RESET_FROM_ETC   = (1<<5),
};


bool    resetInit(void);
void    resetLog(void);
void    resetToBoot(uint8_t mode);
uint8_t resetGetBits(void);
void    resetSetBits(uint8_t data);
void    resetSetMode(uint8_t mode);
uint8_t resetGetMode(void);

#endif


#ifdef __cplusplus
}
#endif

#endif