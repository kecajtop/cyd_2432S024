#ifndef sd_card_csv_h
#define sd_card_csv_h

#define SDSCK 18//14
#define SDMOSI 23//15
#define SDMISO 19//2
#define SDCS 5//13

#include "Arduino.h"

uint32_t find_pin(uint8_t z, uint8_t p);
void convert2pattern1();
void convert2pattern();
void clear_tables();


int find_pin_position(int _pin);
int find_pin_positionA(int _pin);
int find_pin_positionB(int _pin);

void clear_pin_position(int _pos1, int _pos2);

void clear_table_with_pattern(uint32_t _patternA, uint32_t _patternB);

#endif