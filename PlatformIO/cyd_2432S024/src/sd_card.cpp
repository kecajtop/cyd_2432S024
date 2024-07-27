#include "Arduino.h"
#include <SD.h>
#include <Update.h>
#include "macros.h"
#include "lvgl.h"
#include "misc/lv_event.h"
#include "sd_card.h"

const char *CSV_EXT = "p.csv";
int const file_size =12;
char fname[file_size];

extern char list[512];


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
      File root;
      root = SD.open("/");
      Serial.println("[I] SD Card Directory list");
      printDirectory(root, 0);
      return 1;
    }
    return 0;
}

void removeSubstr (char *string, const char *sub) {
    char *match;
    int len = strlen(sub);
    while ((match = strstr(string, sub))) {
        *match = '\0';
        strcat(string, match+len);
    }
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    strcpy(fname, entry.name());
    
    if (strcmp(CSV_EXT, &fname[strlen(fname)-strlen(CSV_EXT)]) == 0)
  {
    removeSubstr(fname, CSV_EXT);
    Serial.print(fname);
        strcat(list, fname);
        strcat(list, "\n");
      //Serial.print(entry.name());
      if (entry.isDirectory()) {
        Serial.println("/");
        printDirectory(entry, numTabs + 1);
      } else {
        Serial.print("\t\t");
        Serial.println(entry.size(), DEC);
      }
    }
    entry.close();
  }
}
