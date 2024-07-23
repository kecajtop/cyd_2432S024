#ifndef config_h
#define config_h

#include "Arduino.h"

#define _CONFIG_VERSION "R1"
#define _CONFIG_START 1

#define _FW_VERSION "1.00"

#define _PCB_VERSION "REV1"

#define CONFIG_FILE_NAME "/config.txt"

#define KEY_MAX_LENGTH    30 // change it if key is longer
#define VALUE_MAX_LENGTH  30 // change it if value is longer

struct config_t
{
    int enable_wifi;
    int enable_selftest;
    int auto_load;
    String file_to_load;
    String ssid_wifi;
    String password_wifi;
};

struct pattern_struct_t //place for hard coded pattern
{
  uint32_t pattern[128][4];
  char version_of_program[9] = _CONFIG_VERSION;
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