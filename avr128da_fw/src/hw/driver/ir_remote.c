#include "ir_remote.h"
#include "cli.h"
#include "qblock.h"


#define IR_REMOTE_BIT_BUF_MAX       32
#define US_TO_TIME(x)               (((x)*15L)/10)  // Clock 1.5Mhz


typedef enum
{
  IR_REMOTE_BIT_NORMAL,
  IR_REMOTE_BIT_REPEAT,
  IR_REMOTE_BIT_HIGH,
  IR_REMOTE_BIT_LOW,
  IR_REMOTE_BIT_LAST,
  IR_REMOTE_BIT_MAX,
} IrRemoteBitValue_t;



typedef struct 
{
  uint16_t on_time;
  uint16_t total_time;
} ir_remote_bit_t;

typedef struct 
{
  IrRemoteBitValue_t value;
  uint16_t on_time;
  uint16_t on_error;
  uint16_t total_time;
  uint16_t total_error;
} ir_remote_bit_ref_t;


static ir_remote_bit_t bit_buf[IR_REMOTE_BIT_BUF_MAX];
static qblock_t rx_bit_q;


#ifdef _USE_HW_CLI
static void cliIrRemote(cli_args_t *args);
#endif


ir_remote_bit_ref_t bit_ref[] = 
{
  {IR_REMOTE_BIT_NORMAL, US_TO_TIME(9000), US_TO_TIME(500), US_TO_TIME(13500), US_TO_TIME(500)},
  {IR_REMOTE_BIT_REPEAT, US_TO_TIME(9000), US_TO_TIME(500), US_TO_TIME(11250), US_TO_TIME(500)},
  {IR_REMOTE_BIT_HIGH,   US_TO_TIME(560),  US_TO_TIME(100), US_TO_TIME(2250),  US_TO_TIME(100)},
  {IR_REMOTE_BIT_LOW,    US_TO_TIME(560),  US_TO_TIME(100), US_TO_TIME(1120),  US_TO_TIME(100)},
  {IR_REMOTE_BIT_LAST,   0,  0, 0, 0},
};


IrRemoteBitValue_t irRemoteGetBit(ir_remote_bit_ref_t *p_ref, ir_remote_bit_t *p_bit);





bool irRemoteInit(void)
{
  qblockCreateBySize(&rx_bit_q, (uint8_t *)bit_buf, sizeof(ir_remote_bit_t), IR_REMOTE_BIT_BUF_MAX);

  PORTC.DIRCLR = (1<<4); // PC4 Input Mode  
  PORTC.PIN4CTRL = (1<<PORT_INVEN_bp)  // Input and output values are inverted
                 | (0<<PORT_ISC0_bp);  // Interrupt disabled but digital input buffer enabled
  
  // TCB1 
  TCB1.CTRLA = (2 << TCB_CLKSEL0_bp);  // TCA0, 1.5Mhz
  TCB1.CTRLB = (3 << TCB_CNTMODE0_bp); // Input Capture Frequency Measurement mode
  
  TCB1.EVCTRL = (1 << TCB_FILTER_bp)  // enables the Input Capture Noise Cancellation unit.
              | (0 << TCB_EDGE_bp)
              | (1 << TCB_CAPTEI_bp); // enables the input capture event.

  TCB1.INTCTRL = (1 << TCB_OVF_bp)    // enables interrupt on overflow
               | (1 << TCB_CAPT_bp);  // enables interrupt on capture        
  TCB1.CTRLA |= (1<<TCB_ENABLE_bp);


  EVSYS.CHANNEL2 = EVSYS_CHANNEL2_PORTC_PIN4_gc;
  EVSYS.USERTCB1CAPT = EVSYS_USER_CHANNEL2_gc;


#ifdef _USE_HW_CLI
  cliAdd("ir_remote", cliIrRemote);
#endif

  return true;
}


IrRemoteBitValue_t irRemoteGetBit(ir_remote_bit_ref_t *p_ref, ir_remote_bit_t *p_bit)
{
  IrRemoteBitValue_t ret = IR_REMOTE_BIT_LAST;

  for (int i=0; i<IR_REMOTE_BIT_LAST; i++)
  {
    bool in_range = true;

    //if (p_bit->on_time <= (p_ref[i].on_time - p_ref[i].on_error)) in_range = false;
    //if (p_bit->on_time >= (p_ref[i].on_time + p_ref[i].on_error)) in_range = false;

    if (p_bit->total_time <= (p_ref[i].total_time - p_ref[i].total_error)) in_range = false;
    if (p_bit->total_time >= (p_ref[i].total_time + p_ref[i].total_error)) in_range = false;

    if (in_range == true)
    {
      ret = p_ref[i].value;
      break;
    }
  }

  return ret;
}


ISR(TCB1_INT_vect, ISR_BLOCK)
{  
  ir_remote_bit_t bit_time;

  if (TCB1.INTFLAGS & (1<<TCB_OVF_bp))
  {
    TCB1.INTCTRL &= ~(1 << TCB_OVF_bp);        
  }

  if (TCB1.INTFLAGS & (1<<TCB_CAPT_bp))
  {
    TCB1.INTCTRL |= (1 << TCB_OVF_bp);

    bit_time.on_time = TCB1.CCMP;
    bit_time.total_time = TCB1.CCMP;
    qblockWrite(&rx_bit_q, (uint8_t *)&bit_time, 1);      
  }

  TCB1.INTFLAGS = TCB1.INTFLAGS;
}


#ifdef _USE_HW_CLI
void cliIrRemote(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "bits") == true)
  {
    uint16_t index = 0;
    ir_remote_bit_t data;

    while(cliKeepLoop())
    {
      if (qblockAvailable(&rx_bit_q) > 0)
      {
        qblockRead(&rx_bit_q, (uint8_t *)&data, 1);

        IrRemoteBitValue_t bit_value;
        bit_value = irRemoteGetBit(bit_ref, &data);
        if (bit_value != IR_REMOTE_BIT_LAST)
        {
          if (bit_value == IR_REMOTE_BIT_NORMAL || bit_value == IR_REMOTE_BIT_REPEAT)
          {
            index = 0;
          }

          cliPrintf_P(PSTR("%4d : on %5d, t %5d, bit : "), index, data.on_time, data.total_time);

          if (bit_value == IR_REMOTE_BIT_NORMAL) cliPrintf_P(PSTR("Normal"));
          if (bit_value == IR_REMOTE_BIT_REPEAT) cliPrintf_P(PSTR("Repeat"));
          if (bit_value == IR_REMOTE_BIT_HIGH)   cliPrintf_P(PSTR("1"));
          if (bit_value == IR_REMOTE_BIT_LOW)    cliPrintf_P(PSTR("0"));
          cliPrintf_P(PSTR("\n"));          
        }
      }
    }
    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("ir_remote bits\n"));
  }
}
#endif
