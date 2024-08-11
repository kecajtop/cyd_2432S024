#include "Adafruit_TCS34725.h"
#include "TCS3472.h"
#include "macros.h"

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))


//
// Gain/time combinations to use and the min/max limits for hysteresis
// that avoid saturation. They should be in order from dim to bright.
//
// Also set the first min count and the last max count to 0 to indicate
// the start and end of the list.
//
const tcs34725::tcs_agc tcs34725::agc_lst[] = {
  { TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_614MS,     0, 63000 },
  //{ TCS34725_GAIN_60X, TCS34725_INTEGRATIONTIME_154MS,  0, 63000 }//,
  //{ TCS34725_GAIN_16X, TCS34725_INTEGRATIONTIME_154MS, 16790, 63000 },
  //{ TCS34725_GAIN_4X,  TCS34725_INTEGRATIONTIME_154MS, 0, 65535 },
  //{ TCS34725_GAIN_1X,  TCS34725_INTEGRATIONTIME_154MS, 15740, 0 }
};
tcs34725::tcs34725() : agc_cur(0), isAvailable(0), isSaturated(0) {
}

// initialize the sensor
boolean tcs34725::begin(void) {
  tcs = Adafruit_TCS34725(agc_lst[agc_cur].at, agc_lst[agc_cur].ag);
  if ((isAvailable = tcs.begin()))
    setGainTime();
  return(isAvailable);
}

// Set the gain and integration time
void tcs34725::setGainTime(void) {
  tcs.setGain(agc_lst[agc_cur].ag);
  tcs.setIntegrationTime(agc_lst[agc_cur].at);
  atime = int(agc_lst[agc_cur].at);
  atime_ms = ((256 - atime) * 2.4);
  switch(agc_lst[agc_cur].ag) {
  case TCS34725_GAIN_1X:
    againx = 1;
    break;
  case TCS34725_GAIN_4X:
    againx = 4;
    break;
  case TCS34725_GAIN_16X:
    againx = 16;
    break;
  case TCS34725_GAIN_60X:
    againx = 60;
    break;
  }
}

// Retrieve data from the sensor and do the calculations
void tcs34725::getData(void) {
  // read the sensor and autorange if necessary
  tcs.getRawData(&r, &g, &b, &c);
  while(1) {
    if (agc_lst[agc_cur].maxcnt && c > agc_lst[agc_cur].maxcnt)
      agc_cur++;
    else if (agc_lst[agc_cur].mincnt && c < agc_lst[agc_cur].mincnt)
      agc_cur--;
    else break;
    if (agc_cur<0) agc_cur=0;
    if (agc_cur>=1) agc_cur=0;
    setGainTime();
    delay((256 - atime) * 2.4 * 2); // shock absorber
    tcs.getRawData(&r, &g, &b, &c);
    break;
  }

  // DN40 calculations
  ir = (r + g + b > c) ? (r + g + b - c) / 2 : 0;
  r_comp = r;// - ir;
  g_comp = g;// - ir;
  b_comp = b;// - ir;
  c_comp = c;// - ir;
  cratio = float(ir) / float(c);

  saturation = ((256 - atime) > 63) ? 65535 : 1024 * (256 - atime);
  saturation75 = (atime_ms < 150) ? (saturation - saturation / 4) : saturation;
  isSaturated = (atime_ms < 150 && c > saturation75) ? 1 : 0;
  cpl = (atime_ms * againx) / (TCS34725_GA * TCS34725_DF);
  maxlux = 65535 / (cpl * 3);

  lux = (TCS34725_R_Coef * float(r_comp) + TCS34725_G_Coef * float(g_comp) + TCS34725_B_Coef * float(b_comp)) / cpl;
  ct = TCS34725_CT_Coef * float(b_comp) / float(r_comp) + TCS34725_CT_Offset;

  r_ratio  = ((float)r_comp / (float)c_comp);
  g_ratio  = ((float)g_comp / (float)c_comp);
  b_ratio  = ((float)b_comp / (float)c_comp);
  
  float max = MAX(MAX(float(r_ratio),float(g_ratio)),float(b_ratio));
  float min = MIN(MIN(float(r_ratio),float(g_ratio)),float(b_ratio));
    
  hue = sat = lum = (max + min) / 2;

  if (max == min) {
    hue = sat = 0; // achromatic
  }
  else 
  {
    float d = max - min;
    //sat = (lum > 0.5) ? d / (2 - max - min) : d / (max + min);
    
    if (d == 0)
    { 
      sat =0;
    }
    else
    {
      sat = d/(1-abs((2*lum)-1));
    }

    if (max == r_ratio) {
      hue = (g_ratio - b_ratio) / d + (g_ratio < b_ratio ? 6 : 0);
      print_kln("max == r_ratio");
    }
    else if (max == g_ratio) {
      hue = 2 + (b_ratio - r_ratio) / d;
      print_kln("max == g_ratio");
    }
    else if (max == b_ratio) {
      hue = 4 + (r_ratio - g_ratio) / d;
      print_kln("max == b_ratio");
    }
    hue /= 6;
    if (hue < 0)
    {
      hue += 1;
    } 
  }
}