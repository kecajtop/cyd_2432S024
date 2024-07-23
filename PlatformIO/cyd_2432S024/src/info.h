#ifndef info_h
#define info_h

#include "Arduino.h"
#include <rom/rtc.h>

void reset_info(void);

void print_reset_reason(RESET_REASON reason);

const char * build_date(void);

void app_info(void);

void lib_info(void);

#endif