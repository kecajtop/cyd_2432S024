#ifndef sd_card_h
#define sd_card_h

#define SDSCK 18//14
#define SDMOSI 23//15
#define SDMISO 19//2
#define SDCS 5//13

#include "Arduino.h"

int sd_init();
void printDirectory(File dir, int numTabs);

#endif