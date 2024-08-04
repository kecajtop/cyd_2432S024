#ifndef ams_h
#define ams_h

#include "Arduino.h"

#define SENSOR_MAX 5000

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define BLACK   0x0000
#define GRAY    0x8410
#define WHITE   0xFFFF
#define RED     0xF800
#define ORANGE  0xFA60
#define YELLOW  0xFFE0  
#define LIME    0x07FF
#define GREEN   0x07E0
#define CYAN    0x07FF
#define AQUA    0x04FF
#define BLUE    0x001F
#define MAGENTA 0xF81F
#define PINK    0xF8FF

void init_ams(void);

void show_graph(void);

void show_colour(void);

void led_on(void);

void led_off(void);

void led_toggle(void);

float RGB_To_HSL(float red, float green, float blue);

#endif