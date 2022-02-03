#include "gpio.h"
#include "cli.h"


#ifdef _USE_HW_GPIO

typedef struct
{
  PORT_t  *port;
  uint8_t  pin;
  uint8_t  mode;
  uint8_t  init_value;
} gpio_tbl_t;


const gpio_tbl_t gpio_tbl[GPIO_MAX_CH] =
    {
        {&PORTF, 2,  _DEF_OUTPUT, _DEF_HIGH },      // 0. LCD DC
        {&PORTA, 7,  _DEF_OUTPUT, _DEF_HIGH },      // 1. LCD CS
    };



#ifdef _USE_HW_CLI
static void cliGpio(cli_args_t *args);
#endif



bool gpioInit(void)
{
  bool ret = true;


  for (int i=0; i<GPIO_MAX_CH; i++)
  {
    gpioPinMode(i, gpio_tbl[i].mode);
    gpioPinWrite(i, gpio_tbl[i].init_value);
  }

#ifdef _USE_HW_CLI
  cliAdd("gpio", cliGpio);
#endif

  return ret;
}

bool gpioPinMode(uint8_t ch, uint8_t mode)
{
  bool ret = true;
  register8_t *p_reg;


  if (ch >= GPIO_MAX_CH)
  {
    return false;
  }

  switch(mode)
  {
    case _DEF_INPUT:
      gpio_tbl[ch].port->DIRCLR = (1<<gpio_tbl[ch].pin);
      break;

    case _DEF_INPUT_PULLUP:
      gpio_tbl[ch].port->DIRCLR = (1<<gpio_tbl[ch].pin);
      p_reg = (register8_t *)&gpio_tbl[ch].port->PIN0CTRL;
      p_reg[gpio_tbl[ch].pin] = (1<<PORT_PULLUPEN_bp);      // Enable Pull-up
      break;

    case _DEF_INPUT_PULLDOWN:
      gpio_tbl[ch].port->DIRCLR = (1<<gpio_tbl[ch].pin);
      break;

    case _DEF_OUTPUT:
      gpio_tbl[ch].port->DIRSET = (1<<gpio_tbl[ch].pin);
      break;

    case _DEF_OUTPUT_PULLUP:
      gpio_tbl[ch].port->DIRSET = (1<<gpio_tbl[ch].pin);
      p_reg = (register8_t *)&gpio_tbl[ch].port->PIN0CTRL;
      p_reg[gpio_tbl[ch].pin] = (1<<PORT_PULLUPEN_bp);      // Enable Pull-up
      break;

    case _DEF_OUTPUT_PULLDOWN:
      gpio_tbl[ch].port->DIRSET = (1<<gpio_tbl[ch].pin);
      break;
  }

  return ret;
}

void gpioPinWrite(uint8_t ch, uint8_t value)
{
  if (ch >= GPIO_MAX_CH)
  {
    return;
  }

  if (value)
  {
    gpio_tbl[ch].port->OUTSET = (1<<gpio_tbl[ch].pin);
  }
  else
  {
    gpio_tbl[ch].port->OUTCLR = (1<<gpio_tbl[ch].pin);
  }
}

uint8_t gpioPinRead(uint8_t ch)
{
  uint8_t ret = _DEF_LOW;

  if (ch >= GPIO_MAX_CH)
  {
    return false;
  }

  if (gpio_tbl[ch].port->IN & (1<<gpio_tbl[ch].pin))
  {
    ret = _DEF_HIGH;
  }

  return ret;
}

void gpioPinToggle(uint8_t ch)
{
  if (ch >= GPIO_MAX_CH)
  {
    return;
  }

  gpio_tbl[ch].port->OUTTGL = (1<<gpio_tbl[ch].pin);
}





#ifdef _USE_HW_CLI
void cliGpio(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "show") == true)
  {
    while(cliKeepLoop())
    {
      for (int i=0; i<GPIO_MAX_CH; i++)
      {
        cliPrintf_P(PSTR("%d"), gpioPinRead(i));
      }
      cliPrintf_P(PSTR("\n"));
      delay(100);
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "read") == true)
  {
    uint8_t ch;

    ch = (uint8_t)args->getData(1);

    while(cliKeepLoop())
    {
      cliPrintf_P(PSTR("gpio read %d : %d\n"), ch, gpioPinRead(ch));
      delay(100);
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    uint8_t ch;
    uint8_t data;

    ch   = (uint8_t)args->getData(1);
    data = (uint8_t)args->getData(2);

    gpioPinWrite(ch, data);

    cliPrintf_P(PSTR("gpio write %d : %d\n"), ch, data);
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("gpio show\n"));
    cliPrintf_P(PSTR("gpio read ch[0~%d]\n"), GPIO_MAX_CH-1);
    cliPrintf_P(PSTR("gpio write ch[0~%d] 0:1\n"), GPIO_MAX_CH-1);
  }
}
#endif


#endif