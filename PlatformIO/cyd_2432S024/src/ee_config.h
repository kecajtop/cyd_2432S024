#ifndef ee_config_h
#define ee_config_h

#include "Arduino.h"

#define i2c_address 0x50

void ee_init();

int ee_info();

void dumpEEPROM(uint16_t memoryAddress, uint16_t length);

uint32_t save_pattern();

uint32_t load_pattern();

uint32_t erase_mode();

#endif