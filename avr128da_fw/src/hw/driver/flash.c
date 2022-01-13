#include "flash.h"
#include "cli.h"

#ifdef _USE_HW_FLASH

#define FLASH_ADDR_OFFSET         0x4000
#define FLASH_MAX_SIZE            (112L*1024L)
#define FLASH_PAGE_SIZE           (512)
#define FLASH_PAGE_COUNT          (FLASH_MAX_SIZE/FLASH_PAGE_SIZE)


static bool is_log = false;


static bool flashInPage(uint32_t page_num, uint32_t addr, uint32_t length);
static bool flashInRange(uint32_t addr_begin, uint32_t length);


#ifdef _USE_HW_CLI
static void cliFlash(cli_args_t *args);
#endif




bool flashInit(void)
{

#ifdef _USE_HW_CLI
  cliAdd("flash", cliFlash);
#endif
  return true;
}

bool flashInRange(uint32_t addr_begin, uint32_t length)
{
  bool ret = false;
  uint32_t addr_end;
  uint32_t flash_start;
  uint32_t flash_end;


  addr_end = addr_begin + length - 1;


  flash_start = FLASH_ADDR_OFFSET;
  flash_end   = FLASH_ADDR_OFFSET + FLASH_MAX_SIZE;
  if ((addr_begin >= flash_start) && (addr_begin < flash_end) &&
      (addr_end   >= flash_start) && (addr_end   < flash_end))
  {
    ret = true;
  }

  return ret;
}

bool flashErase(uint32_t addr, uint32_t length)
{
  bool ret = true;
  int16_t  start_page_num = -1;
  uint32_t page_count = 0;
  uint32_t flash_addr;
  uint32_t i_start;

  if (flashInRange(addr, length) != true)
  {
    return false;
  }

  i_start = addr/FLASH_PAGE_SIZE;
  for (int i=i_start; i<FLASH_PAGE_COUNT; i++)
  {
    if (flashInPage(i, addr, length) == true)
    {
      if (start_page_num < 0)
      {
        start_page_num = i;
      }
      page_count++;            
    }
    else if (page_count > 0)
    {
      break;
    }
  }

  if (page_count > 0)
  {
    while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm) {;}               // Wait for completion of previous command 
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);   // Clear the current command    
    _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);  // Erase the flash page       

    for (int i=0; i<page_count; i++)
    {
      flash_addr = (uint32_t)(start_page_num + i) * FLASH_PAGE_SIZE;
      pgm_word_write(flash_addr, 0x00);                         // Dummy write to start erase operation      

      if (is_log == true)
        cliPrintf_P(PSTR("Erased Addr 0x%X \n"), flash_addr); 
    }

    if (NVMCTRL.STATUS & NVMCTRL_ERROR_gm)
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }

  return ret;
}

bool flashWrite(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint16_t data;


  if (addr%2 != 0)
  {
    return false;
  }

  while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm) {;}               // Wait for completion of previous command 
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);  // Clear the current command      
  _PROTECTED_WRITE_SPM(NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);  // Enable flash Write Mode 

  for (int i=0; i<length; i+=2)
  {

    data  = p_data[i+0] << 0;
    data |= p_data[i+1] << 8;

    pgm_word_write(addr + i, data);
  }
  if (NVMCTRL.STATUS & NVMCTRL_ERROR_gm)
  {
    ret = false;
  }

  return ret;
}

bool flashRead(uint32_t addr, uint8_t *p_data, uint32_t length)
{
  bool ret = true;

  for (int i=0; i<length; i++)
  {
    p_data[i] = pgm_read_byte_far(addr + i);
  }

  return ret;
}

bool flashInPage(uint32_t page_num, uint32_t addr, uint32_t length)
{
  bool ret = false;

  uint32_t page_start;
  uint32_t page_end;
  uint32_t flash_start;
  uint32_t flash_end;


  page_start = page_num * FLASH_PAGE_SIZE;
  page_end   = page_start + FLASH_PAGE_SIZE - 1;
  flash_start  = addr;
  flash_end    = addr + length - 1;


  if (page_start >= flash_start && page_start <= flash_end)
  {
    ret = true;
  }

  if (page_end >= flash_start && page_end <= flash_end)
  {
    ret = true;
  }

  if (flash_start >= page_start && flash_start <= page_end)
  {
    ret = true;
  }

  if (flash_end >= page_start && flash_end <= page_end)
  {
    ret = true;
  }

  return ret;
}






#ifdef _USE_HW_CLI
void cliFlash(cli_args_t *args)
{
  bool ret = false;



  if (args->argc == 1 && args->isStr(0, "info") == true)
  {
    cliPrintf_P(PSTR("flash addr : 0x%X\n"), FLASH_ADDR_OFFSET);
    cliPrintf_P(PSTR("flash len  : %d KB\n"), FLASH_MAX_SIZE/1024L);
    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "read") == true)
  {
    uint32_t addr;
    uint32_t length;
    uint8_t  rd_data;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    for (int i=0; i<length; i++)
    {      
      flashRead(addr + i, &rd_data, 1);
      cliPrintf_P(PSTR("0x%X : "), addr+i);
      cliPrintf_P(PSTR(" 0x%02X\n"), rd_data);
    }

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "erase") == true)
  {
    uint32_t addr;
    uint32_t length;

    addr   = (uint32_t)args->getData(1);
    length = (uint32_t)args->getData(2);

    is_log = true;
    if (flashErase(addr, length) == true)
    {
      cliPrintf_P(PSTR("Erase OK\n"));
    }
    else
    {
      cliPrintf_P(PSTR("Erase Fail\n"));
    }
    is_log = false;

    ret = true;
  }

  if (args->argc == 3 && args->isStr(0, "write") == true)
  {
    uint32_t addr;
    uint32_t data;

    addr   = (uint32_t)args->getData(1);
    data   = (uint32_t)args->getData(2);

    if (flashWrite(addr, (uint8_t *)&data, 4) == true)
    {
      cliPrintf_P(PSTR("Write OK\n"));
    }
    else
    {
      cliPrintf_P(PSTR("Write Fail\n"));
    }

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("flash info\n"));
    cliPrintf_P(PSTR("flash read  addr length\n"));
    cliPrintf_P(PSTR("flash erase addr length\n"));
    cliPrintf_P(PSTR("flash write addr data\n"));
  }
}
#endif

#endif