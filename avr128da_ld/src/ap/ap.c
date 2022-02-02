/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"
#include "boot.h"


enum
{
  MODE_CLI,
  MODE_DOWN
};

enum
{
  ARG_OPTION_PORT = (1<<0),
  ARG_OPTION_BAUD = (1<<1),
  ARG_OPTION_FILE = (1<<2),
  ARG_OPTION_RUN  = (1<<3),
};


typedef struct
{
  uint8_t mode;

  uint32_t arg_bits;
  char     port_str[128];
  uint32_t port_baud;
  char     file_str[128];
  uint32_t addr_fw;
  bool     run_fw;
  uint32_t addr_ver;  
} arg_option_t;



arg_option_t arg_option;


void apShowHelp(void);
bool apGetOption(int argc, char *argv[]);
void apCliMode(void);
void apDownMode(void);
int32_t getFileSize(char *file_name);
int32_t getFileVersion(char *file_name, firm_ver_t *p_ver);
uint32_t getFileCrc(char *file_name);



void apInit(void)
{
  logPrintf("avr128da-ld V220122R1\n\n");

  arg_option.mode = MODE_DOWN;

  cliOpen(_DEF_UART1, 115200);

  cliRunInit();
}

void apMain(int argc, char *argv[])
{
  bool ret;

  ret = apGetOption(argc, argv);
  if (ret != true)
  {
    apExit();
    return;
  }

  if (arg_option.mode == MODE_CLI)
    apCliMode();
  else
    apDownMode();

  apExit();
}

bool apGetOption(int argc, char *argv[])
{
  int opt;
  bool ret = true;
  

  arg_option.addr_ver = 0x00EC;
  arg_option.addr_fw = 0x4000;
  arg_option.run_fw = false;
  arg_option.arg_bits = 0;

  while((opt = getopt(argc, argv, "hcp:b:f:a:rv:")) != -1)
  {
    switch(opt)
    {
      case 'h':
        apShowHelp();
        ret = false;
        break;

      case 'c':
        arg_option.mode = MODE_CLI;
        break;

      case 'p':
        arg_option.arg_bits |= ARG_OPTION_PORT;
        strncpy(arg_option.port_str, optarg, 128);
        logPrintf("-p %s\n", arg_option.port_str);
        break;

      case 'b':
        arg_option.arg_bits |= ARG_OPTION_BAUD;
        arg_option.port_baud = (uint32_t)strtoul((const char * )optarg, (char **)NULL, (int) 0);
        logPrintf("-b %d\n", arg_option.port_baud);
        break;

      case 'f':
        arg_option.arg_bits |= ARG_OPTION_FILE;
        strncpy(arg_option.file_str, optarg, 128);
        logPrintf("-f %s\n", arg_option.file_str);
        break;

      case 'a':
        arg_option.addr_fw = (uint32_t)strtoul((const char * )optarg, (char **)NULL, (int) 0);
        logPrintf("-a 0x%X\n", arg_option.addr_fw);
        break;

      case 'v':
        arg_option.addr_ver = (uint32_t)strtoul((const char * )optarg, (char **)NULL, (int) 0);
        logPrintf("-v 0x%X\n", arg_option.addr_ver);
        break;

      case 'r':
        arg_option.run_fw = true;
        logPrintf("-r 1\n");
        break;

      case '?':
        logPrintf("Unknown\n");
        break;
    }
  }

  return ret;
}

void apShowHelp(void)
{
  logPrintf("avr128da-ld \n");
  logPrintf("            -h : help\n");
  logPrintf("            -c : cli mode\n");
  logPrintf("            -p com1  : com port\n");
  logPrintf("            -b 115200: baud\n");
  logPrintf("            -f fw.bin: firmware\n");
  logPrintf("            -a 0x4000: firmware addr\n");
  logPrintf("            -v 0x00EC: version addr\n");
  logPrintf("            -r       : run firmware\n");
}

void apCliMode(void)
{
  logPrintf("Cli Mode Start...\n");

  while(1)
  {
    cliMain();
  }
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fclose(fp);
  }

  return ret;
}

int32_t getFileVersion(char *file_name, firm_ver_t *p_ver)
{
  int32_t ret = -1;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    fseek( fp, arg_option.addr_ver, SEEK_SET );
    ret = fread(p_ver, 1, sizeof(firm_ver_t), fp);
    fclose(fp);
  }

  return ret;
}

uint32_t getFileCrc(char *file_name)
{
  uint32_t ret = 0;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    uint8_t buffer[1024];
    int len;
    uint16_t calc_crc = 0;

    while(1)
    {
      len = fread(buffer, 1, 1024, fp);
      if (len > 0)
      {
        for (int i=0; i<len; i++)
        {
          utilUpdateCrc(&calc_crc, buffer[i]);
        }
      }
      else
      {
        break;
      }
    }
    ret = calc_crc;

    fclose(fp);
  }

  return ret;
}

void apDownMode(void)
{
  uint32_t arg_check = ARG_OPTION_PORT | ARG_OPTION_BAUD | ARG_OPTION_FILE;

  logPrintf("\n");

  if ((arg_option.arg_bits & arg_check) != arg_check)
  {
    if ((arg_option.arg_bits & ARG_OPTION_PORT) == 0)
    {
      logPrintf("-p port empty\n");
    }
    if ((arg_option.arg_bits & ARG_OPTION_BAUD) == 0)
    {
      logPrintf("-b baud empty\n");
    }
    if ((arg_option.arg_bits & ARG_OPTION_FILE) == 0)
    {
      logPrintf("-f file empty\n");
    }

    return;
  }

  logPrintf("Download Begin..\n");


  uint8_t err_code;
  char *file_name;
  uint32_t addr;
  int32_t file_len;
  uint32_t baud;
  char boot_version[32];
  char boot_name[32];
  char firm_version[32];
  char firm_name[32];
  firm_ver_t firm_ver;
  firm_tag_t firm_tag;
  uint32_t pre_time;

  file_name = arg_option.file_str;
  addr = arg_option.addr_fw;
  baud = arg_option.port_baud;

  bootInit(_USE_UART_CMD, arg_option.port_str, baud);


  file_len = getFileSize(file_name);

  logPrintf("file_name : %s \n", file_name);
  logPrintf("file_addr : 0x%X \n", addr);
  logPrintf("file_len  : %d Bytes\n", file_len);

  if (file_len <= 0)
  {
    logPrintf("File not available\n");
    apExit();
  }

  firm_tag.magic_number = TAG_MAGIC_NUMBER;
  firm_tag.fw_addr = arg_option.addr_fw;
  firm_tag.fw_crc = 0;
  firm_tag.fw_size = file_len;
  firm_tag.fw_crc = getFileCrc(file_name);

  logPrintf("file_crc  : 0x%04X\n", firm_tag.fw_crc);

  getFileVersion(file_name, &firm_ver);
  if (firm_ver.magic_number != VERSION_MAGIC_NUMBER)
  {
    firm_ver.version_str[0] = 0;
    firm_ver.name_str[0] = 0;
  }
  logPrintf("file ver  : %s\n", firm_ver.version_str);    
  logPrintf("file name : %s\n", firm_ver.name_str);  


  FILE *fp;

  if ((fp = fopen(file_name, "rb")) == NULL)
  {
    logPrintf("Unable to open %s\n", file_name);
    apExit();
  }

  while(1)
  {
    if (file_len <= 0) break;


    logPrintf("\n");

    // Read Boot Version
    //
    err_code = bootCmdReadBootVersion(boot_version);
    if (err_code == CMD_OK)
    {
      logPrintf("boot ver  : %s\n", boot_version);    
    }

    err_code = bootCmdReadBootName(boot_name);
    if (err_code == CMD_OK)
    {
      logPrintf("boot name : %s\n", boot_name);    
    }

    logPrintf("\n");

    // Read Firm Version
    //
    err_code = bootCmdReadFirmVersion(firm_version);
    if (err_code == CMD_OK)
    {
      logPrintf("firm ver  : %s\n", firm_version);    
    }

    err_code = bootCmdReadFirmName(firm_name);
    if (err_code == CMD_OK)
    {
      logPrintf("firm name : %s\n", firm_name);    
    }

    logPrintf("\n");

    // 1. Flash Erase
    //
    pre_time = millis();
    err_code = bootCmdFlashErase(addr, file_len, 5000);
    if (err_code != CMD_OK)
    {
      logPrintf("erase Fail : %d\n", err_code);      
      break;
    }
    logPrintf("erase     : OK, %dms\n", millis()-pre_time);    


    // 2. Flash Write
    //
    uint32_t tx_block_size = 256;
    uint8_t  tx_buf[256];
    uint32_t tx_len;
    int32_t  len_to_send;
    bool write_done = false;
    
    tx_len = 0;    
    pre_time = millis();
    while(tx_len < file_len)
    {
      len_to_send = fread(tx_buf, 1, tx_block_size, fp);
      if (len_to_send <= 0)
      {
        break;
      }

      err_code = bootCmdFlashWrite(addr + tx_len, tx_buf, len_to_send, 500);
      if (err_code != CMD_OK)
      {
        logPrintf("write Fail : %d, 0x%X\n", err_code, addr + tx_len);        
        break;
      }
      else
      {
        logPrintf("write     : %d%%\r", ((tx_len+len_to_send)*100/file_len));
      }

      tx_len += len_to_send;    

      if (tx_len == file_len)
      {
        write_done = true;
        break;
      }      
    }

    logPrintf("\n");

    if (write_done == true)
    {
      logPrintf("write     : OK, %dms\n", millis()-pre_time);      

      
      err_code = bootCmdTagWrite(&firm_tag);
      if (err_code != CMD_OK)
      {
        logPrintf("tag write : Fail, %d\n", err_code);
        break;
      }
      else
      {
        logPrintf("tag write : OK\n");
      }

      pre_time = millis();
      err_code = bootCmdTagVerify(1000);
      if (err_code != CMD_OK)
      {
        logPrintf("tag verify: Fail, %d, %dms\n", err_code, millis()-pre_time);
        break;
      }
      else
      {
        logPrintf("tag verify: OK, %dms\n", millis()-pre_time);
      }

      err_code = bootCmdTagWrite(&firm_tag);
      if (err_code != CMD_OK)
      {
        break;
      }

      if (arg_option.run_fw == true)
      {
        err_code = bootCmdJumpToFw();
        logPrintf("jump fw   : OK\n");        
      }
    }
    break;
  }

  fclose(fp);

  logPrintf("Download End..");
}

void apExit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    uartClose(i);
  }
  printf("\nexit\n");
  exit(0);
}

