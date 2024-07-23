#include "Arduino.h"
#include "macros.h"
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include "simple_tft.h"
#include "Update.h"

#define PF_SPACE 22
#define PF_FONT 4

#define ST_MSG_X 320/2
#define ST_MSG_Y 4

#define ST_MSG_PF_X 10
#define ST_MSG_PF_Y 15

#define ST_PF_X 310
#define ST_PF_Y ST_MSG_PF_Y

extern TFT_eSPI tft;

char st_msg[64]; // Character to store
char msg[64]; // Character to store 
//char *msg_ptr = NULL; // Pointer to store the character 

int line = 0;

void init_tft()
{
    tft.init();
    tft.setRotation(3);
}
void clear_display(void)
{
    tft.fillScreen(TFT_WHITE);
}

//TL_DATUM = Top left
//TC_DATUM = Top centre
//TR_DATUM = Top right
//ML_DATUM = Middle left
//MC_DATUM = Middle centre
//MR_DATUM = Middle right
//BL_DATUM = Bottom left
//BC_DATUM = Bottom centre
//BR_DATUM = Bottom right

void display_off()
{
    tft.fillScreen(TFT_WHITE);
    display_selftest_title(msg);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("GOTOWY", 320/2, 240/2, 4);
}

void display_pass()
{
    tft.fillScreen(TFT_GREEN);
    display_selftest_title(msg);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("OK", 320/2, 240/2, 4);
}

void display_fail()
{
    tft.fillScreen(TFT_RED);
    display_selftest_title(msg);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("NOK", 320/2, 240/2, 4);
}

void display_selftest_msg(const char * _msg1, const char * _msg2)
{

    if (line == 0) 
    {
        clear_display();
        display_selftest_title(st_msg);
    }

    line ++;

    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(TL_DATUM);
    tft.drawString(_msg1, ST_MSG_PF_X, ST_MSG_PF_Y+(line*PF_SPACE), PF_FONT);

    tft.setTextDatum(TR_DATUM);

    tft.setTextColor(TFT_BLACK);
    tft.drawString(_msg2, ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    if (line>=9) 
    {
        line=0;
        delay(1000);
    }
}

void display_selftest_pass_fail(const char * _msg,uint8_t status)
{

    if (line == 0) 
    {
        clear_display();
        display_selftest_title(st_msg);
    }

    line ++;

    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(TL_DATUM);
    tft.drawString(_msg, ST_MSG_PF_X, ST_MSG_PF_Y+(line*PF_SPACE), PF_FONT);

    tft.setTextDatum(TR_DATUM);
    if(status == ST_NOK)
    {
        tft.setTextColor(TFT_RED);
        tft.drawString("NOK", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if (status == ST_OK)
    {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("OK", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_PRESENT)
    {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("OBECNY", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_NOT_PRESENT)
    {
        tft.setTextColor(TFT_RED);
        tft.drawString("BRAK", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_DISABLED)
    {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("DISABLED", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_ENABLED)
    {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("ENABLED", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_CONNECTED)
    {
        tft.setTextColor(TFT_GREEN);
        tft.drawString("CONNECTED", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else if(status == ST_DISCONNECTED)
    {
        tft.setTextColor(TFT_RED);
        tft.drawString("DISCONNECTED", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }
    else
    {
        tft.setTextColor(TFT_BLACK);
        tft.drawString("N/A", ST_PF_X, ST_PF_Y+(line*PF_SPACE), PF_FONT);
    }

    if (line>=9) 
    {
        line=0;
        delay(1000);
    }
}

void set_selftest_tite(const char * _msg)
{
    strcpy(st_msg, _msg);
    line=0;
}

void set_tite(const char * _msg)
{
    strcpy(msg, _msg);
    line=0;
}

void clear_display_selftest_pass_fail()
{
    clear_display();
    line = 0;
}

void display_title(const char * _msg)
{
    tft.setTextColor(TFT_BLUE, TFT_BLUE); // Do not plot the background colour
    tft.setTextDatum(TC_DATUM);
    tft.drawString(_msg, ST_MSG_X, ST_MSG_Y, PF_FONT);
}

void display_selftest_title(const char * _msg)
{
    tft.setTextColor(TFT_BLUE, TFT_BLUE); // Do not plot the background colour
    tft.setTextDatum(TC_DATUM);
    tft.drawString(_msg, ST_MSG_X, ST_MSG_Y, PF_FONT);
}

void display_firmware_update(void)
{
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("FIRMWARE UPGRADE", 320/2, 240/2, 4);
}

void display_firmware_update_ok(void)
{
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("OK", 320/2, 240/2, 4);
}

void display_firmware_update_fail(void)
{
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("FAIL", 320/2, 240/2, 4);
}

void display_firmware_update_restart(void)
{
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    tft.drawString("RESTART", 320/2, 240/2, 4);
}

#define PBSTR "####################"
#define PBWIDTH 20

void display_firmware_update_progress(size_t currSize, size_t totalSize)
{
    float progress = (float)currSize / (float)totalSize;
    tft.setTextColor(TFT_BLACK, TFT_RED); // Do not plot the background colour
    tft.setTextDatum(MC_DATUM);
    char numAsString[64];
    tft.setTextPadding(64);

    sprintf(numAsString, "Process at %d of %d bytes", currSize, totalSize); 
    tft.drawString(numAsString, 320/2, (240/2)+40, 2); 

    int val = (int) (progress * 100.);
    int lpad = (int) (progress * PBWIDTH);
    int rpad = PBWIDTH - lpad;

    sprintf(numAsString, "\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, ""); 
    tft.drawString(numAsString, 320/2, (240/2)+60, 2);
}
