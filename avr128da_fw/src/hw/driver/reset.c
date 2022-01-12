#include "reset.h"
#include "cli.h"


#ifdef _USE_HW_RESET

static uint8_t reset_mode = 0x00;

static uint8_t reset_bits = 0x00;
static uint8_t reset_reg  = 0x00;



static void cliReset(cli_args_t *args);


bool resetInit(void)
{
  reset_reg = RSTCTRL.RSTFR;

  if (reset_reg & RSTCTRL_PORF_bm)
    reset_bits |= RESET_FROM_POWER;

  if (reset_reg & RSTCTRL_BORF_bm)
    reset_bits |= RESET_FROM_BOD;

  if (reset_reg & RSTCTRL_EXTRF_bm)
    reset_bits |= RESET_FROM_PIN;

  if (reset_reg & RSTCTRL_WDRF_bm)
    reset_bits |= RESET_FROM_WDG;

  if (reset_reg & RSTCTRL_SWRF_bm)
    reset_bits |= RESET_FROM_SOFT;

  if (reset_reg & RSTCTRL_UPDIRF_bm)
    reset_bits |= RESET_FROM_ETC;


  #ifdef _USE_HW_CLI
  cliAdd("reset", cliReset);
  #endif

  return true;
}

void resetLog(void)
{
  logPrintf_P(PSTR("\n"));
  logPrintf_P(PSTR("ResetMode : 0x%02X\r\n"), reset_mode);

  if (reset_bits & RESET_FROM_POWER)
  {
    logPrintf_P(PSTR("ResetFrom : Power\r\n"));
  }
  if (reset_bits & RESET_FROM_PIN)
  {
    logPrintf_P(PSTR("ResetFrom : Pin\r\n"));
  }
  if (reset_bits & RESET_FROM_WDG)
  {
    logPrintf_P(PSTR("ResetFrom : Wdg\r\n"));
  }
  if (reset_bits & RESET_FROM_SOFT)
  {
    logPrintf_P(PSTR("ResetFrom : Soft\r\n"));
  }
  if (reset_bits & RESET_FROM_BOD)
  {
    logPrintf_P(PSTR("ResetFrom : BoD\r\n"));
  }  
  if (reset_bits & RESET_FROM_ETC)
  {
    logPrintf_P(PSTR("ResetFrom : Etc\r\n"));
  }  
}

void resetToBoot(uint8_t mode)
{
  reset_mode = mode;

  // Soft Reset
  _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRST_bm);
}

uint8_t resetGetBits(void)
{
  return reset_bits;
}

void resetSetBits(uint8_t data)
{
  reset_bits = data;
}

void resetSetMode(uint8_t mode)
{
  reset_mode = mode;
}

uint8_t resetGetMode(void)
{
  return reset_mode;
}


#ifdef _USE_HW_CLI
void cliReset(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    resetLog();
    ret = true;
  }
  if (args->argc == 2 && args->isStr(0, "boot") == true)
  {
    uint8_t boot_mode;

    delay(10);
    boot_mode = (uint8_t)args->getData(1);
    resetToBoot(boot_mode);    
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("reset info\n"));
    cliPrintf_P(PSTR("reset boot 0x00~0xFF\n"));
  }
}
#endif

#endif