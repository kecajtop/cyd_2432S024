#include "Arduino.h"
#include <rom/rtc.h>
#include "macros.h"
#include "lvgl.h"
#include "config.h"
#include "simple_tft.h"
#include "info.h"

const char compile_date[] = __DATE__ " " __TIME__;

void lib_info(void)
{
    String LVGL_Arduino = "\tLVGL: ";
    display_selftest_msg("LVGL", LVGL_Arduino.c_str());
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    print_kln( LVGL_Arduino );
}

void app_info(void)
{	
  infoln("INFO ");
  print_k("\t");
  print_kln(STR(APP_INFO));
  //display_selftest_msg("APP_INFO", STR(APP_INFO));
  print_k("\tFW_VERSION: ");
  print_kln(_FW_VERSION_ESP32);
  display_selftest_msg("\tFIRMWARE", _FW_VERSION_ESP32);
  print_k("\tPCB_VERSION: ");
  print_kln(_PCB_VERSION);
  display_selftest_msg("PCB", _PCB_VERSION);
}

const char * build_date(void)
{
	print_k("\tBuild date: ");
  print_kln(compile_date);
  return compile_date;
}

void reset_info(void)
{
    msg("CPU0 reset reason: ");
    print_reset_reason(rtc_get_reset_reason(0));
    msg("CPU1 reset reason: ");
    print_reset_reason(rtc_get_reset_reason(1));
}

void print_reset_reason(RESET_REASON reason)
{
  switch ( reason)
  {
    case 1 : print_kln ("POWERON_RESET");break;          /**<1, Vbat power on reset*/
    case 3 : print_kln ("SW_RESET");break;               /**<3, Software reset digital core*/
    case 4 : print_kln ("OWDT_RESET");break;             /**<4, Legacy watch dog reset digital core*/
    case 5 : print_kln ("DEEPSLEEP_RESET");break;        /**<5, Deep Sleep reset digital core*/
    case 6 : print_kln ("SDIO_RESET");break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 : print_kln ("TG0WDT_SYS_RESET");break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 : print_kln ("TG1WDT_SYS_RESET");break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 : print_kln ("RTCWDT_SYS_RESET");break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : print_kln ("INTRUSION_RESET");break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : print_kln ("TGWDT_CPU_RESET");break;       /**<11, Time Group reset CPU*/
    case 12 : print_kln ("SW_CPU_RESET");break;          /**<12, Software reset CPU*/
    case 13 : print_kln ("RTCWDT_CPU_RESET");break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : print_kln ("EXT_CPU_RESET");break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : print_kln ("RTCWDT_BROWN_OUT_RESET");break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : print_kln ("RTCWDT_RTC_RESET");break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : print_kln ("NO_MEAN");
  }
}