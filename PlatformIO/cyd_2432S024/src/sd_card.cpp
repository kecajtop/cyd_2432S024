#include "Arduino.h"
#include <SD.h>
#include <Update.h>
#include "macros.h"
#include "sd_card.h"

SPIClass sdSPI(HSPI);

int sd_init()
{
    sdSPI.begin(SDSCK, SDMISO, SDMOSI, SDCS);

    if (!SD.begin(SDCS, sdSPI)) 
    {
      Serial.println("[E] Card failed, or not present");
      return 0;
    }
    else
    { 
      Serial.println("[I] Card initialized.");
      return 1;
    }
    return 0;
}