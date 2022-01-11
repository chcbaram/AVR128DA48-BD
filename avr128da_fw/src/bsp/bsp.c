#include "bsp.h"


static volatile uint32_t ms_cnt = 0;

ISR(TCB0_INT_vect, ISR_NOBLOCK)
{
  ms_cnt++;
  TCB0.INTFLAGS = (1<<TCB_CAPT_bp);
}






bool bspInit(void)
{
  if (FUSE.BOOTSIZE > 0)
  {
    // Interrupt Vector for BOOTCODE
    _PROTECTED_WRITE(CPUINT.CTRLA, (CPUINT.CTRLA | CPUINT_IVSEL_bm));
  }

  // Clocks
  //  
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
  

  // Timers
  TCB0.CTRLA   = (0x00 << TCB_CLKSEL_gp);               // DIV1 = CLK_PER = 24Mhz
  TCB0.CTRLB   = (0x00 << TCB_CNTMODE_gp);              // Periodic Interrupt Mode
  TCB0.INTCTRL = (1 << TCB_CAPT_bp);                    // Capture Interrupt Enable
  TCB0.CCMP    = (1000*(F_CPU/1000000)) - 1;            // 1ms
  TCB0.CTRLA  |= (1 << TCB_ENABLE_bp);                  // Timer Start


  sei();

  return true;
}

void delay(uint32_t ms)
{
  uint32_t pre_time = millis();

  while(millis()-pre_time < ms)
  {    
  }
}

uint32_t millis(void)
{
  uint32_t ret;
  uint8_t old_sreg = SREG;

  cli();
  ret = ms_cnt;
  SREG = old_sreg;

  return ret;
}