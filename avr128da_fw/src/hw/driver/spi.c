#include "spi.h"
#include "cli.h"
#include "gpio.h"


typedef enum
{
  SPI_HW_SPI, 
  SPI_HW_UART,
} SpiType_t;


typedef struct
{
  bool is_open;
  bool is_tx_done;
  bool is_error;

  SpiType_t spi_type;
  int8_t gpio_cs;

  void (*func_tx)(void);

  SPI_t *h_spi;
} spi_t;


static spi_t spi_tbl[SPI_MAX_CH];
static USART_t *h_uart = &USART3;



#ifdef _USE_HW_CLI
static void cliSpi(cli_args_t *args);
#endif

bool spiInit(void)
{
  bool ret = true;


  for (int i=0; i<SPI_MAX_CH; i++)
  {
    spi_tbl[i].is_open = false;
    spi_tbl[i].is_tx_done = true;
    spi_tbl[i].is_error = false;
    spi_tbl[i].func_tx = NULL;
    spi_tbl[i].spi_type = SPI_HW_SPI;
  }

#ifdef _USE_HW_CLI
  cliAdd("spi", cliSpi);
#endif
  return ret;
}

bool spiBegin(uint8_t ch)
{
  bool ret = false;
  spi_t *p_spi = &spi_tbl[ch];
  SPI_t *h_spi;
  uint32_t f_clk_per = F_CPU;
  uint32_t sample_per_bit = 2;
  uint16_t ubrr;

  switch(ch)
  {
    case _DEF_SPI1:
      h_spi = &SPI0;
      p_spi->h_spi = h_spi;
      p_spi->spi_type = SPI_HW_SPI;


      PORTA.DIRSET = (1<<4); // PA4 MOSI Output
      PORTA.DIRSET = (1<<6); // PA6 SCK  Output

      h_spi->CTRLA = 0;
      h_spi->CTRLB = 0;
      h_spi->INTCTRL = 0;

      h_spi->CTRLA = (1 << SPI_MASTER_bp)     // SPI Host mode
                   | (0 << SPI_DORD_bp)       // MSB
                   | (1 << SPI_CLK2X_bp)      // 24Mhz x 2 = 48Mhz
                   | (1 << SPI_PRESC0_bp);    // DIV4, 48Mhz/8 = 6Mhz

      h_spi->CTRLB = (1 << SPI_BUFEN_bp)       // Buffer Mode Enable
                   | (1 << SPI_SSD_bp)         // Disable the Client Select line when operating as SPI host 
                   | (0 << SPI_MODE0_bp);      // SPIO MODE0


      h_spi->CTRLA |= (1 << SPI_ENABLE_bp);

      p_spi->gpio_cs = 1;      
      p_spi->is_open = true;
      ret = true;

      spiSetCS(p_spi->gpio_cs, _DEF_HIGH);
      break;

    case _DEF_SPI2:          
      p_spi->spi_type = SPI_HW_UART;  

      ubrr = (f_clk_per/sample_per_bit) / 12000000;                 // 12Mbps
      h_uart->BAUD = ubrr<<6;

      h_uart->STATUS = h_uart->STATUS;
      h_uart->CTRLC = (3 << USART_CMODE_gp)            // Host SPI
                    | (0 << USART_UDORD_bp)            // MSb of the data word is transmitted first
                    | (0 << USART_UCPHA_bp);           // Data are sampled on the leading (first) edge

      PORTB.DIRSET = (1<<0); // PB0 TXD Output
      PORTB.DIRCLR = (1<<1); // PB1 RXD Input
      PORTB.DIRSET = (1<<2); // PB2 XCK Output

      h_uart->CTRLB = (1 << USART_RXEN_bp)             // Receiver Enable
                    | (1 << USART_TXEN_bp)             // Transmitter Enable
                    | (0 << USART_RXMODE_gp);          // Receiver Mode = Normal Mode

      h_uart->CTRLA = (0 << USART_RXCIE_bp);           // Receive Complete Interrupt Disable


      p_spi->gpio_cs = 2;      
      p_spi->is_open = true;
      ret = true;
      break;      
  }

  return ret;
}

void spiSetDataMode(uint8_t ch, uint8_t dataMode)
{
  SPI_t *h_spi;

  if (ch >= SPI_MAX_CH)
  {
    return;
  }

  if (spi_tbl[ch].spi_type == SPI_HW_SPI)
  {
    h_spi = spi_tbl[ch].h_spi;
    h_spi->CTRLB &= ~(0x3 << SPI_MODE0_bp);
    h_spi->CTRLB |= (dataMode << SPI_MODE0_bp);
  }
}

void spiSetBitWidth(uint8_t ch, uint8_t bit_width)
{  
  // TODO
}

void spiSetBaudRate(uint8_t ch, uint32_t baud_rate_hz)
{
  // TODO  
}
uint32_t spiGetBaudRate(uint8_t ch)
{
  // TODO
  return 0;
}

void spiSetCS(uint8_t ch, uint8_t value)
{
  if (ch >= SPI_MAX_CH)
  {
    return;
  }  
  if (spi_tbl[ch].gpio_cs < 0)
  {
    return;
  }
  gpioPinWrite(spi_tbl[ch].gpio_cs, value);
}

bool spiTransfer_SPI(uint8_t ch, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout)
{
  bool ret = true;
  spi_t *p_spi = &spi_tbl[ch];
  SPI_t *h_spi;
  uint32_t pre_time;
  uint32_t tx_i = 0;
  uint32_t rx_i = 0;
  const size_t fifo_depth = 1;

  if (p_spi->is_open == false) return false;

  
  uint16_t rx_remaining = length;
  uint16_t tx_remaining = length;

  h_spi = p_spi->h_spi;
  h_spi->INTFLAGS = h_spi->INTFLAGS;

  pre_time = millis();
  while (rx_remaining || tx_remaining) 
  {
    if (tx_remaining) 
    {
      if (h_spi->INTFLAGS & (1<<SPI_DREIF_bp) && rx_remaining < (tx_remaining + fifo_depth))
      {
        if (tx_buf == NULL)
        {
          h_spi->DATA = 0xFF;        
        }
        else
        {
          h_spi->DATA = tx_buf[tx_i];
        }
        tx_i++;
        --tx_remaining;
      }
    }

    if (rx_remaining) 
    {
      if (h_spi->INTFLAGS & (1<<SPI_RXCIF_bp))
      {
        if (rx_buf == NULL)
        {
          (void)h_spi->DATA;
        }
        else
        {
          rx_buf[rx_i] = h_spi->DATA;
        }
        --rx_remaining;
        rx_i++;
      }
    }

    if (millis()-pre_time >= timeout)
    {
      ret = false;
      break;
    }
  }

  while(1)
  {
    if (h_spi->INTFLAGS & (1<<SPI_TXCIF_bp))
    {
      break;
    }
    if (millis()-pre_time >= timeout)
    {
      ret = false;
      break;
    }
  }

  return ret;
}

bool spiTransfer_UART(uint8_t ch, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout)
{
  bool ret = true;
  spi_t *p_spi = &spi_tbl[ch];
  uint32_t pre_time;
  uint32_t tx_i = 0;
  uint32_t rx_i = 0;
  const size_t fifo_depth = 1;

  if (p_spi->is_open == false) return false;

  
  uint16_t rx_remaining = length;
  uint16_t tx_remaining = length;

  h_uart->STATUS = h_uart->STATUS;
      

  pre_time = millis();
  while (rx_remaining || tx_remaining) 
  {
    if (tx_remaining) 
    {
      if (h_uart->STATUS & (1<<USART_DREIF_bp) && rx_remaining < (tx_remaining + fifo_depth))
      {
        if (tx_buf == NULL)
        {
          h_uart->TXDATAL = 0xFF;        
        }
        else
        {
          h_uart->TXDATAL = tx_buf[tx_i];
        }
        tx_i++;
        --tx_remaining;
      }
    }

    if (rx_remaining) 
    {
      if (h_uart->STATUS & (1<<USART_RXCIF_bp))
      {
        if (rx_buf == NULL)
        {
          (void)h_uart->RXDATAL;
        }
        else
        {
          rx_buf[rx_i] = h_uart->RXDATAL;
        }
        --rx_remaining;
        rx_i++;
      }
    }

    if (millis()-pre_time >= timeout)
    {
      ret = false;
      break;
    }
  }

  while(1)
  {
    if (h_uart->STATUS & (1<<USART_TXCIF_bp))
    {
      break;
    }
    if (millis()-pre_time >= timeout)
    {
      ret = false;
      break;
    }
  }

  return ret;
}

bool spiTransfer(uint8_t ch, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout)
{
  if (spi_tbl[ch].spi_type == SPI_HW_SPI)
  {
    return spiTransfer_SPI(ch, tx_buf, rx_buf, length, timeout);
  }
  else
  {
    return spiTransfer_UART(ch, tx_buf, rx_buf, length, timeout);
  }
}

uint8_t spiTransfer8(uint8_t ch, uint8_t data)
{
  uint8_t ret;
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false) return 0;

  spiTransfer(ch, &data, &ret, 1, 10);

  return ret;  
}

uint16_t spiTransfer16(uint8_t ch, uint16_t data)
{
  uint8_t tBuf[2];
  uint8_t rBuf[2];
  uint16_t ret;
  spi_t  *p_spi = &spi_tbl[ch];


  if (p_spi->is_open == false) return 0;


  tBuf[1] = (uint8_t)data;
  tBuf[0] = (uint8_t)(data>>8);
  spiTransfer(ch, &tBuf[0], &rBuf[0], 2, 10);

  ret = rBuf[0];
  ret <<= 8;
  ret += rBuf[1];

  return ret;  
}

void spiAttachTxInterrupt(uint8_t ch, void (*func)())
{
  // TODO
}


#ifdef _USE_HW_CLI

void cliSpi(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 2 && args->isStr(0, "begin"))
  {
    uint8_t spi_ch;

    spi_ch = constrain(args->getData(1), 0, SPI_MAX_CH-1);
    if (spiBegin(spi_ch) == true)
    {
      cliPrintf_P(PSTR("spi begin %d OK\n"), spi_ch);
    }
    else
    {
      cliPrintf_P(PSTR("spi begin %d Fail\n"), spi_ch);
    }
    ret = true;
  }

  if (args->argc == 2 && args->isStr(0, "read"))
  {
    uint8_t spi_ch;
    uint8_t rx_data = 0;
    bool spi_ret;

    spi_ch = constrain(args->getData(1), 0, SPI_MAX_CH-1);
    spiSetCS(spi_ch, _DEF_LOW);
    spi_ret = spiTransfer(spi_ch, NULL, &rx_data, 1, 100);
    spiSetCS(spi_ch, _DEF_HIGH);
    if (spi_ret == true)
    {
      cliPrintf_P(PSTR("rx_data 0x%X OK\n"), rx_data);
    }
    else
    {
      cliPrintf_P(PSTR("rx_data Fail\n"));
    }
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write"))
  {
    uint8_t spi_ch;
    uint8_t tx_data;
    bool spi_ret;

    spi_ch = constrain(args->getData(1), 0, SPI_MAX_CH-1);
    tx_data = args->getData(2);

    spiSetCS(spi_ch, _DEF_LOW);
    spi_ret = spiTransfer(spi_ch, &tx_data, NULL, 1, 100);
    spiSetCS(spi_ch, _DEF_HIGH);
    if (spi_ret == true)
    {
      cliPrintf_P(PSTR("tx_data 0x%X OK\n"), tx_data);
    }
    else
    {
      cliPrintf_P(PSTR("tx_data Fail\n"));
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("spi begin 0~%d\n"), SPI_MAX_CH-1);
    cliPrintf_P(PSTR("spi read ch[0~%d]\n"), SPI_MAX_CH-1);
    cliPrintf_P(PSTR("spi write ch[0~%d] data\n"), SPI_MAX_CH-1);
  }
}
#endif
