#include "buzzer.h"
#include "cli.h"
#include "sw_timer.h"


static bool is_on = false;
static uint16_t on_time_cnt = 0;


#ifdef _USE_HW_CLI
static void cliBuzzer(cli_args_t *args);
#endif


void buzzerISR(void *arg)
{
  if (is_on && on_time_cnt > 0)
  {
    on_time_cnt--;

    if (on_time_cnt == 0)
    {
      buzzerOff();
    }
  }
}




bool buzzerInit(void)
{
  PORTA.DIRSET = (1<<2);
  PORTA.OUTCLR = (1<<2);
  
  TCA0.SINGLE.CTRLA = (4 << TCA_SINGLE_CLKSEL0_bp);     // DIV16, 24Mhz/16 = 1.5Mhz
  TCA0.SINGLE.CTRLB = (0 << TCA_SINGLE_CMP2EN_bp)       // Compare 2 Disable
                    | (1 << TCA_SINGLE_WGMODE0_bp);     // Frequency Mode 
  TCA0.SINGLE.CTRLC = (0 << TCA_SINGLE_CMP2OV_bp);

  TCA0.SINGLE.CMP0 = 0xFFFF/2;
  TCA0.SINGLE.CTRLA |= (1<<TCA_SINGLE_ENABLE_bp);

  buzzerOff();  


  uint8_t timer_ch;
  timer_ch = swTimerGetHandle();
  swTimerSet(timer_ch, 1, LOOP_TIME, buzzerISR, NULL);
  swTimerStart(timer_ch);


#ifdef _USE_HW_CLI
  cliAdd("buzzer", cliBuzzer);
#endif  
  return true;
}

void buzzerOn(uint16_t freq_hz, uint16_t time_ms)
{
  uint32_t freq_cmp;

  freq_cmp = F_CPU/(2*16)/freq_hz;

  TCA0.SINGLE.CMP0 = freq_cmp - 1;  
  TCA0.SINGLE.CTRLB |= (1 << TCA_SINGLE_CMP2EN_bp);      // Compare 2 Enable
  is_on = true;

  uint8_t old_sreg = SREG;
  cli();
  on_time_cnt = time_ms;
  SREG = old_sreg;
}

void buzzerOff(void)
{  
  TCA0.SINGLE.CTRLB &= ~(1 << TCA_SINGLE_CMP2EN_bp);     // Compare 2 Disable
  is_on = false;
}

#ifdef _USE_HW_CLI
void cliBuzzer(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 3 && args->isStr(0, "on"))
  {
    uint16_t freq_hz;
    uint16_t time_ms;

    freq_hz = args->getData(1);
    time_ms = args->getData(2);

    buzzerOn(freq_hz, time_ms);
    ret = true;
  }
  
  if (args->argc == 1 && args->isStr(0, "off"))
  {
    buzzerOff();
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "test"))
  {
    uint16_t freq_tbl[8] = {261, 293, 329, 349, 391, 440, 493, 523};
    uint8_t freq_i;

    for (int i=0; i<16; i++)
    {
      if (i/8 == 0)
      {
        freq_i = i%8;
      }
      else
      {
        freq_i = 7 - (i%8);
      }
      buzzerOn(freq_tbl[freq_i], 150);
      cliPrintf_P(PSTR("%dHz, %dms\n"), freq_tbl[freq_i], 100);
      delay(300);
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("buzzer on freq(32~500000) time_ms\n"));
    cliPrintf_P(PSTR("buzzer off\n"));
    cliPrintf_P(PSTR("buzzer test\n"));
  }
}
#endif
