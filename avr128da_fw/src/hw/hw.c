#include "hw.h"



volatile const firm_ver_t firm_ver __attribute__((section(".vectors"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
};


extern bool ssd1306Init(void);

bool hwInit(void)
{
  bspInit();
  
  #ifdef _USE_HW_CLI
  cliInit();  
  #endif
  gpioInit();
  ledInit();
  infoInit();
  uartInit();
  spiInit();
  buttonInit();
  resetInit();
  flashInit();  
  lcdInit();  

  sei();  
  return true;
}

void jumpToApp(void)
{
  cli();

  pgm_jmp_far(FLASH_ADDR_FW/sizeof(uint16_t));  
}

void jumpToBoot(void)
{
  resetToBoot(RESET_MODE_RUN_BOOT);
}