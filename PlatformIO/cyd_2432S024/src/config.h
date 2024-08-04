#ifndef config_h
#define config_h

#include "Arduino.h"
#include "ESP32OTAPull.h"

#if __has_include("update_settings.h") // optionally override with values in settings.h
#include "update_settings.h"
#else
#define JSON_URL   "https://raw.githubusercontent.com/kecajtop/cyd_2432S024/main/json/cyd_2432S024.json" // this is where you'll post your JSON filter file
#define _FW_VERSION_ESP32    "1.0.0" // The current version of this program
#define _SD_FW_NAME_ESP32 "/firmwareESP32.bin"
#define _SD_FW_NAME_ESP32_BAK "/firmwareESP32.bak"
#define _PCB_VERSION "REV1"
#define CONFIG_FILE_NAME "/config.txt"
#endif

#define KEY_MAX_LENGTH    30 // change it if key is longer
#define VALUE_MAX_LENGTH  30 // change it if value is longer

struct config_t
{
    int enable_wifi;
    int enable_selftest;
    int auto_load;
    int start;
    String file_to_load;
    String ssid_wifi;
    String password_wifi;
};

struct pattern_struct_t //place for hard coded pattern
{
  uint32_t pattern[128][4];
};

String HELPER_ascii2String(char *ascii, int length);
float HELPER_ascii2Float(char *ascii, int length);
int HELPER_ascii2Int(char *ascii, int length);
int SD_findKey(const __FlashStringHelper * key, char * value);
String SD_findString(const __FlashStringHelper * key);
float SD_findFloat(const __FlashStringHelper * key);
int SD_findInt(const __FlashStringHelper * key);
bool SD_available(const __FlashStringHelper * key);
int load_config(void);

#endif