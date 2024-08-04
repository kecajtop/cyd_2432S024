#ifndef sd_fw_upgrade_h
#define sd_fw_upgrade_h

#include "Arduino.h"
#include "ESP32OTAPull.h"

void sd_fw_upgrade(void);

int sd_new_fw(void);

void progressCallBack(size_t currSize, size_t totalSize);

String online_fw_check(void);

const char *errtext(int code);

void callback_percent(int offset, int totallength);

void callback_dots(int offset, int totallength);

#endif