#include "info.h"
#include "cli.h"


#ifdef _USE_HW_INFO

#ifdef _USE_HW_CLI
void cliInfo(cli_args_t *args);
#endif
  
bool infoInit(void)
{
  #ifdef _USE_HW_CLI
  cliAdd("info", cliInfo);
  #endif
  return true;
}

#ifdef _USE_HW_CLI
void cliInfo(cli_args_t *args)
{
  bool ret = false;

  if (args->argc == 1 && args->isStr(0, "fuse") == true)
  {
    cliPrintf_P(PSTR("\nFUSE.WDTCFG      \t : 0x%08X\n"), FUSE.WDTCFG);
    cliPrintf_P(PSTR("     WINDOW[3:0] \t : 0x%X\n"), (FUSE.WDTCFG >> FUSE_WINDOW_gp) & FUSE_WINDOW_gm);
    cliPrintf_P(PSTR("     PERIOD[3:0] \t : 0x%X\n"), (FUSE.WDTCFG >> FUSE_PERIOD_gp) & FUSE_PERIOD_gm);

    uint16_t reg; 

    // FUSE.BODCFG
    //
    cliPrintf_P(PSTR("\nFUSE.BODCFG      \t : 0x%08X\n"), FUSE.BODCFG);

    const char * PROGMEM lvl_tbl[] = {PSTR("1.9V"), PSTR("2.45V"), PSTR("2.70V"), PSTR("2.85V")};
    const char * PROGMEM sampfreq_tbl[] = {PSTR("128Hz"), PSTR("32Hz")};
    const char * PROGMEM active_tbl[] = {PSTR("DISABLE"), PSTR("ENABLE"), PSTR("SAMPLE"), PSTR("ENABLEWAIT")};
    const char * PROGMEM sleep_tbl[] = {PSTR("DISABLE"), PSTR("ENABLE"), PSTR("SAMPLE"), PSTR("Reserved")};

    reg = (FUSE.BODCFG & FUSE_LVL_gm) >> FUSE_LVL_gp;
    cliPrintf_P(PSTR("     LVL         \t : ")); cliPrintf_P(lvl_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.BODCFG & FUSE_SAMPFREQ_bm) >> FUSE_SAMPFREQ_bp;
    cliPrintf_P(PSTR("     SAMPFREQ    \t : ")); cliPrintf_P(sampfreq_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.BODCFG & FUSE_ACTIVE_gm) >> FUSE_ACTIVE_gp;
    cliPrintf_P(PSTR("     ACTIVE      \t : ")); cliPrintf_P(active_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.BODCFG & FUSE_SLEEP_gm) >> FUSE_SLEEP_gp;
    cliPrintf_P(PSTR("     SLEEP       \t : ")); cliPrintf_P(sleep_tbl[reg]); cliPrintf_P(PSTR("\n"));


    // FUSE.OSCCFG
    //
    cliPrintf_P(PSTR("\nFUSE.OSCCFG     \t : 0x%08X\n"), FUSE.OSCCFG);

    const char * PROGMEM clksel_tbl[] = {PSTR("OSCHF"), PSTR("OSC32K")};

    reg = (FUSE.OSCCFG & FUSE_CLKSEL_gm) >> FUSE_CLKSEL_gp;
    cliPrintf_P(PSTR("     CLKSEL     \t : ")); cliPrintf_P(clksel_tbl[reg]); cliPrintf_P(PSTR("\n"));


    // FUSE.SYSCFG0
    //
    cliPrintf_P(PSTR("\nFUSE.SYSCFG0    \t : 0x%08X\n"), FUSE.SYSCFG0);

    const char * PROGMEM crcsrc_tbl[] = {PSTR("FLASH"), PSTR("BOOT"), PSTR("BOOTAPP"), PSTR("NOCRC")};
    const char * PROGMEM crcsel_tbl[] = {PSTR("CRC16-CCITT"), PSTR("CRC32-IEEE 802.3")};
    const char * PROGMEM rstpincfg_tbl[] = {PSTR("INPUT PF6 configured as general input pin"), 
                                            PSTR("- "),
                                            PSTR("RESET External Reset enabled on PF6"),
                                            PSTR("- ")};
    const char * PROGMEM eesave_tbl[] = {PSTR("EEPROM erased during Chip Erase"), PSTR("EEPROM not erased under Chip Erase")};

    reg = (FUSE.SYSCFG0 & FUSE_CRCSRC_gm) >> FUSE_CRCSRC_gp;
    cliPrintf_P(PSTR("     CRCSRC     \t : ")); cliPrintf_P(crcsrc_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.SYSCFG0 & FUSE_CRCSEL_bm) >> FUSE_CRCSEL_bp;
    cliPrintf_P(PSTR("     CRCSEL     \t : ")); cliPrintf_P(crcsel_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.SYSCFG0 & FUSE_RSTPINCFG_gm) >> FUSE_RSTPINCFG_gp;
    cliPrintf_P(PSTR("     RSTPINCFG  \t : ")); cliPrintf_P(rstpincfg_tbl[reg]); cliPrintf_P(PSTR("\n"));
    reg = (FUSE.SYSCFG0 & FUSE_EESAVE_bm) >> FUSE_EESAVE_bp;
    cliPrintf_P(PSTR("     EESAVE     \t : ")); cliPrintf_P(eesave_tbl[reg]); cliPrintf_P(PSTR("\n"));


    // FUSE.SYSCFG0
    //
    cliPrintf_P(PSTR("\nFUSE.SYSCFG1    \t : 0x%08X\n"), FUSE.SYSCFG1);

    const char * PROGMEM sut_tbl[] = {PSTR("0 ms"), PSTR("1 ms"), PSTR("2 ms"), PSTR("4 ms"), 
                                         PSTR("8 ms"), PSTR("16 ms"), PSTR("32 ms"), PSTR("64 ms")};

    reg = (FUSE.SYSCFG1 & FUSE_SUT_gm) >> FUSE_SUT_gp;
    cliPrintf_P(PSTR("     SUT          \t : ")); cliPrintf_P(sut_tbl[reg]); cliPrintf_P(PSTR("\n"));


    // FUSE.BOOTSIZE
    //
    cliPrintf_P(PSTR("\nFUSE.BOOTSIZE   \t : 0x%08X\n"), FUSE.BOOTSIZE);
    cliPrintf_P(PSTR("     BOOTSIZE     \t : %d KB\n"), FUSE.BOOTSIZE*512/1024);

    // FUSE.CODESIZE
    //
    cliPrintf_P(PSTR("\nFUSE.CODESIZE   \t : 0x%08X\n"), FUSE.CODESIZE);
    cliPrintf_P(PSTR("     CODESIZE     \t : %d KB\n"), FUSE.CODESIZE*512/1024);

    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "version") == true)
  {
    firm_ver_t ver;
    
    memcpy_PF(&ver, FLASH_ADDR_FW_VER, sizeof(firm_ver_t));

    cliPrintf_P(PSTR("Magic   : 0x%lX\n"), ver.magic_number);
    cliPrintf_P(PSTR("Version : %s\n"), ver.version_str);
    cliPrintf_P(PSTR("Name    : %s\n"), ver.name_str);

    ret = true;
  }

  if (ret != true)
  {
    cliPrintf_P(PSTR("info fuse\n"));
    cliPrintf_P(PSTR("info version\n"));
  }
}
#endif

#endif