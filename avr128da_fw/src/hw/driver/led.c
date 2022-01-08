#include "led.h"


#ifdef _USE_HW_LED



typedef struct  
{
  uint16_t dir_port;
  uint16_t out_port;
  uint8_t  pin_num;
  uint8_t  on_state;
} led_pin_t;

static const led_pin_t PROGMEM pin_tbl[LED_MAX_CH] = 
{
  {(uint16_t)&PORTC.DIR, (uint16_t)&PORTC.OUT, 6, _DEF_LOW}
};





bool ledInit(void)
{
  volatile uint8_t *reg;


  for (int i=0; i<LED_MAX_CH; i++)
  {
    reg = (volatile uint8_t *)( pgm_read_word(&pin_tbl[i].dir_port));
    *reg |= (1<<pgm_read_byte(&pin_tbl[i].pin_num));
  }

  return true;
}

void ledOn(uint8_t ch)
{
  volatile uint8_t *reg;


  reg = (volatile uint8_t *)( pgm_read_word(&pin_tbl[ch].out_port));

  if (pgm_read_byte(&pin_tbl[ch].on_state) == _DEF_HIGH)
  {
    *reg |= (1<<pgm_read_byte(&pin_tbl[ch].pin_num));
  }
  else
  {    
    *reg &= ~(1<<pgm_read_byte(&pin_tbl[ch].pin_num));
  }
}

void ledOff(uint8_t ch)
{
  volatile uint8_t *reg;


  reg = (volatile uint8_t *)( pgm_read_word(&pin_tbl[ch].out_port));

  if (pgm_read_byte(&pin_tbl[ch].on_state) == _DEF_HIGH)
  {
    *reg &= ~(1<<pgm_read_byte(&pin_tbl[ch].pin_num));
  }
  else
  {    
    *reg |= (1<<pgm_read_byte(&pin_tbl[ch].pin_num));
  }
}

void ledToggle(uint8_t ch)
{
  volatile uint8_t *reg;


  reg = (volatile uint8_t *)( pgm_read_word(&pin_tbl[ch].out_port));

  *reg ^= (1<<pgm_read_byte(&pin_tbl[ch].pin_num));
}

#endif