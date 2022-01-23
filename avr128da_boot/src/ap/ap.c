#include "ap.h"
#include "boot/boot.h"


cmd_t cmd_boot;


void apBoot(void);




void apInit(void)
{  
  #ifdef _USE_HW_CLI
  cliOpen(_DEF_UART1, 115200);   
  #else
  uartOpen(_DEF_UART1, 115200);   
  #endif

  logPrintf_P(PSTR("\nBootloader..\n"));

  apBoot();

  cmdInit(&cmd_boot);
  cmdOpen(&cmd_boot, _DEF_UART1, 115200);
}

void apMain(void)
{
  uint32_t pre_time;


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 100)
    {
      pre_time = millis();

      ledToggle(_DEF_LED1);     
    }
    
    if (cmdReceivePacket(&cmd_boot) == true)
    {
      bootProcessCmd(&cmd_boot);
    }

    #ifdef _USE_HW_CLI
    cliMain();
    #endif
  }
}

void apBoot(void)
{
  bool is_run_boot = false;


  if (resetGetMode() & RESET_MODE_RUN_APP)
  {
    resetSetMode(0x00);
    logPrintf_P(PSTR("\nJump To App Fail..\n"));
    return;
  }

  if (resetGetMode() & RESET_MODE_RUN_BOOT)
  {
    logPrintf_P(PSTR("Request From APP\n"));
    is_run_boot = true;
  }
  if (buttonGetPressed(_DEF_BUTTON1) == true)
  {
    logPrintf_P(PSTR("Request From Button\n"));
    is_run_boot = true;
  }

  if (is_run_boot != true)
  {
    logPrintf_P(PSTR("\nJump To App..\n"));
    delay(10);
    jumpToApp();  
  }
}