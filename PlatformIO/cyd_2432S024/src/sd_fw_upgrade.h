#ifndef sd_fw_upgrade_h
#define sd_fw_upgrade_h

#define FIRMWARE_VERSION 0.2

#include "Arduino.h"

void sd_fw_upgrade(void);

int sd_new_fw(void);

void progressCallBack(size_t currSize, size_t totalSize);

#endif