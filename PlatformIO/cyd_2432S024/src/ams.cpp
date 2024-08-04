#include "Arduino.h"
#include "ams.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "macros.h"
#include "simple_tft.h"
#include "Adafruit_TCS34725.h"
#include "TCS3472.h"

tcs34725 rgb_sensor;
extern TFT_eSPI tft;

//float calibratedValues[AS726x_NUM_CHANNELS];

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
    if(!rgb_sensor.begin())
    {
      display_selftest_pass_fail("TCS3472",ST_NOK);
    }
    else
    {
      display_selftest_pass_fail("TCS3472",ST_OK);
    }
    //barWidth = tft.width() / AS726x_NUM_CHANNELS/2;
}

void show_graph(void)
{
  //if(ams.dataReady())
  //{
        
    //read the values!
    //ams.readRawValues(sensorValues);
  //  ams.readCalibratedValues(calibratedValues);
    
  //  for(int i=0; i<AS726x_NUM_CHANNELS; i++){
  //    uint16_t height = map(calibratedValues[i], 0, SENSOR_MAX, 0, tft.height());

  //    tft.fillRect(barWidth * i, 0, barWidth, tft.height() - height, TFT_BLACK);
  //    tft.fillRect(barWidth * i, tft.height() - height, barWidth, height, colors[i]);
  //  }
  //}
}

void show_colour(void)
{
  
  //float hue, color_value, color_value_sum;
  //float Red_Ratio, Green_Ratio, Blue_Ratio;
  //uint16_t Clear, Red, Green, Blue;
  Serial.println();
  rgb_sensor.getData();
  Serial.print(F("Gain:"));
  Serial.print(rgb_sensor.againx);
  Serial.print(F("x "));
  Serial.print(F("Time:"));
  Serial.print(rgb_sensor.atime_ms);
  Serial.print(F("ms (0x"));
  Serial.print(rgb_sensor.atime, HEX);
  Serial.println(F(")"));

  //Serial.print(F("Raw R:"));
  //Serial.print(rgb_sensor.r);
  //Serial.print(F(" G:"));
 // Serial.print(rgb_sensor.g);
  //Serial.print(F(" B:"));
  //Serial.print(rgb_sensor.b);
  //Serial.print(F(" C:"));
  //Serial.println(rgb_sensor.c);

  Serial.print(F("IR:"));
  Serial.print(rgb_sensor.ir);
  Serial.print(F(" CRATIO:"));
  Serial.print(rgb_sensor.cratio);
  Serial.print(F(" Sat:"));
  Serial.print(rgb_sensor.saturation);
  Serial.print(F(" Sat75:"));
  Serial.print(rgb_sensor.saturation75);
  Serial.print(F(" "));
  Serial.println(rgb_sensor.isSaturated ? "*SATURATED*" : "");

  Serial.print(F("CPL:"));
  Serial.print(rgb_sensor.cpl);
  Serial.print(F(" Max lux:"));
  Serial.println(rgb_sensor.maxlux);

  //Serial.print(F("Compensated R:"));
  //Serial.print(rgb_sensor.r_comp);
  //Serial.print(F(" G:"));
  //Serial.print(rgb_sensor.g_comp);
  //Serial.print(F(" B:"));
  //Serial.print(rgb_sensor.b_comp);
  //Serial.print(F(" C:"));
  //Serial.println(rgb_sensor.c_comp);

  Serial.print(F("Ratio R:"));
  Serial.print(rgb_sensor.r_ratio);
  Serial.print(F(" G:"));
  Serial.print(rgb_sensor.g_ratio);
  Serial.print(F(" B:"));
  Serial.println(rgb_sensor.b_ratio);

  Serial.print(F("Lux:"));
  Serial.print(rgb_sensor.lux);
  Serial.print(F(" CT:"));
  Serial.print(rgb_sensor.ct);
  Serial.println(F("K"));

  Serial.print(F("Hue:"));
  Serial.print(rgb_sensor.hue);
  Serial.print(F(" Sat:"));
  Serial.print(rgb_sensor.sat);
  Serial.print(F(" Lum:"));
  Serial.println(rgb_sensor.lum);

  //  uint32_t sum = Clear;
  //  float r, g, b;
  //  r = Red;
  //  r /= sum;
  //  g = Green;
  //  g /= sum;
  //  b = Blue;
  //  b /= sum;
  //  r *= 31;
  //  g *= 63;
  //  b *= 31;
  
  //Serial.print(F("HUE "));
  //Serial.println(color_value);
  //uint16_t rgb = ((int)r<<11)+((int)g<<5)+(int)b;
  //Serial.print("0x");
  //Serial.print(rgb, HEX);
  //tft.fillRect(0,120,tft.width()/2,tft.height(),rgb);
  

  char str[32];

 // if(ams.dataReady())
  //{
        
    //read the values!
    //ams.readRawValues(sensorValues);
    //ams.readCalibratedValues(calibratedValues);

    //red = calibratedValues[5];
    //green = calibratedValues[2];
    //blue = calibratedValues[1];

    if (rgb_sensor.r_ratio > rgb_sensor.b_ratio && rgb_sensor.r_ratio > rgb_sensor.b_ratio && rgb_sensor.ct < 3500 && rgb_sensor.hue < 0.1 && rgb_sensor.cpl > 2)
    {  
        Serial.println(" - (Red Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_RED);
    }    
    else if (rgb_sensor.b_ratio > rgb_sensor.r_ratio && rgb_sensor.b_ratio > rgb_sensor.g_ratio && rgb_sensor.ct > 9000 && rgb_sensor.hue < 0.1 && rgb_sensor.cpl > 2)   
    {  
        Serial.println(" - (Blue Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_BLUE); 
    }  
    else if (rgb_sensor.g_ratio > rgb_sensor.r_ratio && rgb_sensor.g_ratio > rgb_sensor.b_ratio && rgb_sensor.ct > 5500 && rgb_sensor.ct < 9000 && rgb_sensor.hue > 0.36 && rgb_sensor.cpl > 2 && rgb_sensor.sat > 0.21 && rgb_sensor.lux < 800 && rgb_sensor.ir > 2000)  
    {  
        Serial.println(" - (Green Color)");
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_DARKGREEN);
    } 
    else
    {
        tft.fillRect(0,120,tft.width()/2,tft.height(),TFT_BLACK);
    }
    /*tft.setTextColor(TFT_BLACK, TFT_WHITE,1); // Do not plot the background colour
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
  */
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
    //ams.drvOn();
    msgln("LED is ON");
}

void led_off(void)
{
    //ams.drvOff();
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