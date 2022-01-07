#include <avr/io.h>



int main(void) 
{  
  CCP = CCP_IOREG_gc;
  CLKCTRL.XOSC32KCTRLA = (1 << CLKCTRL_ENABLE_bp);      // XOSC32K Enable

  RTC.CLKSEL = (0x02 << RTC_CLKSEL_gp);                 // RTC for XOSC32K

  CCP = CCP_IOREG_gc;
  CLKCTRL.OSCHFCTRLA = (0x09 << CLKCTRL_FREQSEL_gp)     // OSCHF = 24Mhz
                     | (0x01 << CLKCTRL_AUTOTUNE_bp);   // AUTOTUNE Enable
  CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLA = (0 << CLKCTRL_CLKSEL_gp);         // CLK_MAIN = OSCHF = 24Mhz

  CCP = CCP_IOREG_gc;
  CLKCTRL.MCLKCTRLB = (0 << CLKCTRL_PEN_bp);            // CLK_PER = CLK_MAIN = 24Mhz
  

  PORTC.DIRSET = (1<<6);

  while(1)
  {
    PORTC.OUTTGL = (1<<6);

    for (volatile uint32_t i=0; i<10000*2; i++)
    {
    }    
  }

  return 0;
}
