#ifndef simple_tft_h
#define simple_tft_h

#include "Arduino.h"
#include "Update.h"

#define ST_NOK 0
#define ST_OK 1
#define ST_PRESENT 2
#define ST_NOT_PRESENT 3
#define ST_DISABLED 4
#define ST_ENABLED 5
#define ST_CONNECTED 6
#define ST_DISCONNECTED 7


void init_tft();
void clear_display(void);
void display_selftest_pass_fail(const char * _msg,uint8_t status);
void display_selftest_title(const char * _msg);
void display_selftest_msg(const char * _msg1, const char * _msg2);
void set_selftest_tite(const char * _msg);
void set_tite(const char * _msg);
void display_firmware_update(void);
void display_firmware_update_ok(void);
void display_firmware_update_fail(void);
void display_firmware_update_restart(void);
void display_firmware_update_progress(size_t currSize, size_t totalSize);
void clear_display_selftest_pass_fail();

void display_pass();
void display_fail();
void display_off();

void display_title(const char * _msg);

#endif