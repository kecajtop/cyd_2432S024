#include "Arduino.h"
#include "ams.h"
#include "Adafruit_AS726x.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "macros.h"
#include "simple_tft.h"

Adafruit_AS726x ams;
extern TFT_eSPI tft;

float calibratedValues[AS726x_NUM_CHANNELS];

uint16_t colors[] = {
  MAGENTA,
  BLUE,
  GREEN,
  YELLOW,
  ORANGE,
  RED
};

uint16_t barWidth;

bool led = false;

void init_ams(void)
{
    if(!ams.begin())
    {
      display_selftest_pass_fail("AS7262",ST_NOK);
      while(1);
    }
    else
    {
      display_selftest_pass_fail("AS7262",ST_OK);
      ams.setConversionType(MODE_2);
    }
    barWidth = tft.width() / AS726x_NUM_CHANNELS/2;
}

void show_graph(void)
{
  if(ams.dataReady())
  {
        
    //read the values!
    //ams.readRawValues(sensorValues);
    ams.readCalibratedValues(calibratedValues);
    
    for(int i=0; i<AS726x_NUM_CHANNELS; i++){
      uint16_t height = map(calibratedValues[i], 0, SENSOR_MAX, 0, tft.height());

      tft.fillRect(barWidth * i, 0, barWidth, tft.height() - height, TFT_BLACK);
      tft.fillRect(barWidth * i, tft.height() - height, barWidth, height, colors[i]);
    }
  }
}

void show_colour(void)
{
  float red,blue,green;

  char str[32];

  if(ams.dataReady())
  {
        
    //read the values!
    //ams.readRawValues(sensorValues);
    ams.readCalibratedValues(calibratedValues);

    red = calibratedValues[5];
    green = calibratedValues[2];
    blue = calibratedValues[1];

    if (red > blue && red > green && red > 300)
    {  
        Serial.println(" - (Red Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_RED);
    }    
    else if (blue > red && blue > green && blue > 300)   
    {  
        Serial.println(" - (Blue Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_BLUE); 
    }  
    else if (green > red && green > blue && green > 300)  
    {  
        Serial.println(" - (Green Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_GREEN);
    } 
    else
    {
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_BLACK);
    }
    tft.setTextColor(TFT_BLACK, TFT_WHITE,1); // Do not plot the background colour
    tft.setTextDatum(TL_DATUM);
    tft.setTextPadding(128);
    sprintf(str, "R%.2f", red); 
    tft.drawString(str, 0, 0, 2);
    sprintf(str, "G%.2f", green); 
    tft.drawString(str, 0, 20, 2);
    sprintf(str, "G%.2f", blue); 
    tft.drawString(str, 0, 40, 2);
    sprintf(str, "H%.5f", RGB_To_HSL(red,green,blue)); 
    tft.drawString(str, 0, 60, 2);
  }
}

float RGB_To_HSL(float red, float green, float blue) 
{
  float volatile fmax, fmin, hue, saturation, luminance;
 
  fmax = MAX_FLOAT(MAX_FLOAT(red, green), blue);
  fmin = MIN_FLOAT(MIN_FLOAT(red, green), blue);
 
  luminance = fmax;
  if (fmax > 0)
    saturation = (fmax - fmin) / fmax;
  else
    saturation = 0;
 
  if (saturation == 0)
    hue = 0;
  else {
    if (fmax == red)
      hue = (green - blue) / (fmax - fmin);
    else if (fmax == green)
      hue = 2 + (blue - red) / (fmax - fmin);
    else
      hue = 4 + (red - green) / (fmax - fmin);
    hue = hue / 6;
  
    if (hue < 0)
      hue += 1;
  }
  return hue;
}


void led_on(void)
{
    ams.drvOn();
    msgln("LED is ON");
}

void led_off(void)
{
    ams.drvOff();
    msgln("LED is OFF");
}

void led_toggle(void)
{
    led = !led;

    if (led)
    {
        led_on();
    }
    else
    {
        led_off();
    }
}