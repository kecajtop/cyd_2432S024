#ifndef sd_config_h
#define sd_config_h

#include "Arduino.h"


struct tabela_zlaczy_t
{
  uint16_t numer_zlacza;
  uint16_t liczba_pinow_zlacza;
  char nazwa_zlacza[9];
  uint16_t mapa_pinow_zlacza[40];
};

struct tabela_polaczen_zlaczy_t
{
  uint16_t numer_zlacza;
  uint16_t pin;
};

struct tabela_polaczen_pcfs_t
{
  uint16_t pin;
};

char feedRowParser();

bool rowParserFinished();

int load_csv(const char * f_name);

#endif