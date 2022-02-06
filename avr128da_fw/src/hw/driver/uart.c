#include "uart.h"
#include "qbuffer.h"


#define UART_MODE_POLLING       0
#define UART_MODE_INTERRUPT     1
#define UART_MODE_DMA           2


#define UART_RX_BUF_LENGTH      64


typedef struct
{
  bool     is_open;
  uint32_t baud;
  uint8_t  tx_mode;
  uint8_t  rx_mode;
  bool     one_wire;

  USART_t  *p_handle;
  uint8_t  rx_buf[UART_RX_BUF_LENGTH];

  qbuffer_t qbuffer_rx;
} uart_t;



uart_t   uart_tbl[UART_MAX_CH];




static void uartRxHandler(uint8_t ch, uint8_t rx_data);



bool uartInit(void)
{
  uint8_t i;


  for (i=0; i<UART_MAX_CH; i++)
  {
    uart_tbl[i].is_open = false;
    uart_tbl[i].rx_mode = UART_MODE_POLLING;
    uart_tbl[i].tx_mode = UART_MODE_POLLING;
    uart_tbl[i].one_wire = false;
  }

  return true;
}

bool uartDeInit(void)
{
  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false;
  uart_t *p_uart;
  uint16_t ubrr;
  uint32_t f_clk_per = F_CPU;
  uint32_t sample_per_bit = 16;

  switch(ch)
  {
    case _DEF_UART1:
      p_uart = &uart_tbl[ch];
      p_uart->rx_mode  = UART_MODE_INTERRUPT;
      p_uart->baud     = baud;
      p_uart->p_handle = &USART1;

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);
      

      ubrr = (f_clk_per * (64/sample_per_bit)) / baud;
      p_uart->p_handle->BAUD = ubrr;

      p_uart->p_handle->STATUS = p_uart->p_handle->STATUS;
      p_uart->p_handle->CTRLC = (0x00 << USART_CMODE_gp)            // Asynchronous USART
                              | (0x00 << USART_PMODE_gp)            // Parity Mode = Disabled
                              | (0x01 << USART_SBMODE_bp)           // Stop Bit Mode = 1 Stop bit
                              | (0x03 << USART_CHSIZE_gp);          // Character Size = 8 bit

      PORTC.DIRSET = (1<<0); // PC0 TXD Output
      PORTC.DIRCLR = (1<<1); // PC1 RXD Input

      p_uart->p_handle->CTRLB = (0x01 << USART_RXEN_bp)             // Receiver Enable
                              | (0x01 << USART_TXEN_bp)             // Transmitter Enable
                              | (0x00 << USART_RXMODE_gp);          // Receiver Mode = Normal Mode

      p_uart->p_handle->CTRLA = (0x01 << USART_RXCIE_bp);           // Receive Complete Interrupt Enable

      p_uart->is_open  = true;
      ret = true;
      break;

    case _DEF_UART2:
      p_uart = &uart_tbl[ch];
      p_uart->rx_mode  = UART_MODE_INTERRUPT;
      p_uart->baud     = baud;
      p_uart->p_handle = &USART0;
      p_uart->one_wire = true;

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);
      

      ubrr = (f_clk_per * (64/sample_per_bit)) / baud;
      p_uart->p_handle->BAUD = ubrr;

      p_uart->p_handle->STATUS = p_uart->p_handle->STATUS;
      p_uart->p_handle->CTRLC = (0x00 << USART_CMODE_gp)            // Asynchronous USART
                              | (0x00 << USART_PMODE_gp)            // Parity Mode = Disabled
                              | (0x01 << USART_SBMODE_bp)           // Stop Bit Mode = 1 Stop bit
                              | (0x03 << USART_CHSIZE_gp);          // Character Size = 8 bit
      
      PORTA.DIRSET = (1 << 0); // PA0 Output
      PORTA.PIN0CTRL = (1 << PORT_PULLUPEN_bp); // PA0 Pull-up

      p_uart->p_handle->CTRLB = (0x01 << USART_RXEN_bp)             // Receiver Enable
                              | (0x01 << USART_TXEN_bp)             // Transmitter Enable
                              | (0x01 << USART_ODME_bp)             // Open Drain Mode
                              | (0x00 << USART_RXMODE_gp);          // Receiver Mode = Normal Mode

      p_uart->p_handle->CTRLA = (0x01 << USART_RXCIE_bp)            // Receive Complete Interrupt Enable
                              | (0x01 << USART_TXCIE_bp)            // Transimit Complete Interrupt Enable
                              | (0x01 << USART_LBME_bp);            // Loop-back Mode Enable

      p_uart->is_open  = true;
      ret = true;
      break;

    case _DEF_UART3:
      p_uart = &uart_tbl[ch];
      p_uart->rx_mode  = UART_MODE_INTERRUPT;
      p_uart->baud     = baud;
      p_uart->p_handle = &USART2;

      qbufferCreate(&p_uart->qbuffer_rx, p_uart->rx_buf, UART_RX_BUF_LENGTH);
      

      ubrr = (f_clk_per * (64/sample_per_bit)) / baud;
      p_uart->p_handle->BAUD = ubrr;

      p_uart->p_handle->STATUS = p_uart->p_handle->STATUS;
      p_uart->p_handle->CTRLC = (0x00 << USART_CMODE_gp)            // Asynchronous USART
                              | (0x00 << USART_PMODE_gp)            // Parity Mode = Disabled
                              | (0x01 << USART_SBMODE_bp)           // Stop Bit Mode = 1 Stop bit
                              | (0x03 << USART_CHSIZE_gp);          // Character Size = 8 bit

      PORTF.DIRSET = (1<<4); // PF4 TXD Output
      PORTF.DIRCLR = (1<<5); // PF5 RXD Input

      PORTMUX.USARTROUTEA &= ~(3<<PORTMUX_USART20_bp);
      PORTMUX.USARTROUTEA |=  (1<<PORTMUX_USART20_bp);              // PF4(TXD), PF5(RXD)

      p_uart->p_handle->CTRLB = (0x01 << USART_RXEN_bp)             // Receiver Enable
                              | (0x01 << USART_TXEN_bp)             // Transmitter Enable
                              | (0x00 << USART_RXMODE_gp);          // Receiver Mode = Normal Mode

      p_uart->p_handle->CTRLA = (0x01 << USART_RXCIE_bp);           // Receive Complete Interrupt Enable

      p_uart->is_open  = true;
      ret = true;
      break;      
  }

  return ret;
}

bool uartIsOpen(uint8_t ch)
{
  return uart_tbl[ch].is_open;
}

bool uartClose(uint8_t ch)
{
  bool ret = false;


  if (ch >= UART_MAX_CH)
  {
    return false;
  }


  if (uart_tbl[ch].is_open == true)
  {
    ret = true;
  }

  return ret;
}

uint32_t uartAvailable(uint8_t ch)
{
  if (ch >= UART_MAX_CH)
  {
    return 0;
  }

  return qbufferAvailable(&uart_tbl[ch].qbuffer_rx);
}

bool uartFlush(uint8_t ch)
{
  if(uart_tbl[ch].rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferFlush(&uart_tbl[ch].qbuffer_rx);
  }

  return true;
}

void uartPutch(uint8_t ch, uint8_t data)
{
  uartWrite(ch, &data, 1 );
}

uint8_t uartGetch(uint8_t ch)
{
  uint8_t ret = 0;


  while(1)
  {
    if (uartAvailable(ch) > 0)
    {
      ret = uartRead(ch);
      break;
    }
  }

  return ret;
}

int32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  int32_t ret = 0;
  uint32_t pre_time;
  uint32_t i = 0;
  uart_t *p_uart = &uart_tbl[ch];
  USART_t *p_handle = p_uart->p_handle;

  if (p_uart->one_wire == true)
  {
    p_uart->p_handle->CTRLB &= ~(1 << USART_RXEN_bp); // Receiver Disable
  }

  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    pre_time = millis();
    i = 0;
    while(i<length)
    {
      if (p_handle->STATUS & (1<<USART_DREIF_bp))  
      {
        p_handle->TXDATAL = p_data[i];
        i++;
      }
      
      if (millis()-pre_time >= 500)
      {
        break;
      }      
    }
  }
  ret = i;

  return ret;
}

uint8_t uartRead(uint8_t ch)
{
  uint8_t ret = 0;
  uart_t *p_uart = &uart_tbl[ch];


  if (p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferRead(&p_uart->qbuffer_rx, &ret, 1);
  }


  return ret;
}

int32_t uartPrintf(uint8_t ch, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[128];


  len = vsnprintf(print_buffer, 128, fmt, arg);
  va_end (arg);

  ret = uartWrite(ch, (uint8_t *)print_buffer, len);

  return ret;
}

int32_t uartPrintf_P(uint8_t ch, const char *fmt, ...)
{
  int32_t ret = 0;
  va_list arg;
  va_start (arg, fmt);
  int32_t len;
  char print_buffer[128];


  len = vsnprintf_P(print_buffer, 128, fmt, arg);
  va_end (arg);

  ret = uartWrite(ch, (uint8_t *)print_buffer, len);

  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  return uart_tbl[ch].baud;
}

void uartRxHandler(uint8_t ch, uint8_t rx_data)
{
  uart_t *p_uart = &uart_tbl[ch];


  if(p_uart->rx_mode == UART_MODE_INTERRUPT)
  {
    qbufferWrite(&p_uart->qbuffer_rx, &rx_data, 1);    
  }
}

ISR(USART1_RXC_vect, ISR_NOBLOCK)
{
  uartRxHandler(_DEF_UART1, USART1.RXDATAL);
}

ISR(USART0_RXC_vect, ISR_NOBLOCK)
{
  uartRxHandler(_DEF_UART2, USART0.RXDATAL);
}

ISR(USART0_TXC_vect, ISR_BLOCK)
{
  USART0.CTRLB |= (0x01 << USART_RXEN_bp); // Receiver Enable  
  USART0.STATUS = (1 << USART_TXCIF_bp);   // Clear TXCIF
}

ISR(USART2_RXC_vect, ISR_NOBLOCK)
{
  uartRxHandler(_DEF_UART3, USART2.RXDATAL);
}