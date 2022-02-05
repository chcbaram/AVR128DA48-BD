#include "ap.h"


void ledISR(void *arg);




void apInit(void)
{  
  #ifdef _USE_HW_CLI
  cliOpen(_DEF_UART1, 115200);   
  #endif

  
  logPrintf_P(PSTR("\nFirmware..\n"));

  lcdClearBuffer(black);

  lcdSetFont(LCD_FONT_HAN);
  lcdPrintf_P(2, 2, white, PSTR("한글 English"));  

  lcdSetFont(LCD_FONT_07x10);
  lcdPrintf_P(2, 20, white, PSTR("Font 7x10"));  

  lcdSetFont(LCD_FONT_11x18);
  lcdPrintf_P(2, 32, white, PSTR("Font 7x10"));  

  lcdDrawRect(0, 0, LCD_WIDTH, LCD_HEIGHT, white);
  lcdUpdateDraw();  
  delay(2000);

  lcdClearBuffer(black);
  lcdUpdateDraw();  

  lcdSetFont(LCD_FONT_HAN);

  uint8_t led_ch;

  led_ch = swTimerGetHandle();
  swTimerSet(led_ch, 500, LOOP_TIME, ledISR, NULL);
  swTimerStart(led_ch);
}

void apMain(void)
{
  uint32_t pre_time;


  pre_time = millis();
  while(1)
  {
    if (millis()-pre_time >= 500)
    {
      pre_time = millis();      
    }
    
    #ifdef _USE_HW_CLI
    cliMain();
    #endif
  }
}

void ledISR(void *arg)
{
  ledToggle(_DEF_LED1);
}
