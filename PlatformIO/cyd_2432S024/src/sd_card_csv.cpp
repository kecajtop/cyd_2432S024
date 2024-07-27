#include "Arduino.h"
#include "sd_config.h"
#include "sd_card_csv.h"
#include "config.h"
#include "macros.h"



extern tabela_zlaczy_t tabela_zlaczy[16];

extern tabela_polaczen_zlaczy_t tabela_polaczen[128][8];

extern tabela_polaczen_pcfs_t tabela_polaczen_pcfs[128][8];

extern int stronaA[32];

extern int stronaB[32];

extern pattern_struct_t pattern;

int int_bank = 0;

void clear_tables(void)
{
  for (int i=0;i<16;i++)
  {
    tabela_zlaczy[i].numer_zlacza = 0;
    tabela_zlaczy[i].liczba_pinow_zlacza = 0;
    tabela_zlaczy[i].nazwa_zlacza[0] = '\0';
    for (int j=0;j<40;j++)
    {
      tabela_zlaczy[i].mapa_pinow_zlacza[j] = 0;
    }
  }

  for (int i = 0; i < 128; i++)
  {
    for(int j = 0; j < 8; j++)
    {
      tabela_polaczen[i][j].numer_zlacza = 0;
      tabela_polaczen[i][j].pin = 0;
      tabela_polaczen_pcfs[i][j].pin = 0;
    }
  }
  clear_table_with_pattern(0xFFFFFFFF,0xFFFFFFFF);
}

void clear_table_with_pattern(uint32_t _patternA, uint32_t _patternB)
{
  for (int i = 0; i < 64; i++)
  {
    pattern.pattern[(i*2)][int_bank] = _patternA;
    pattern.pattern[(i*2)+1][int_bank] = _patternB;
  }
  for (int i = 0; i < 64; i++)
  {
    if (i<32)
    {
      bitClear(pattern.pattern[(i*2)][int_bank],i);
    }
    else
    {
      bitClear(pattern.pattern[(i*2+1)][int_bank],i);
    }
  }

}

void convert2pattern1()
{
  for (int i=0;i<16;i++)
  {
    if (tabela_zlaczy[i].numer_zlacza!=0)
    {
      UsrLog(tabela_zlaczy[i].nazwa_zlacza);
      for (int u=0;u<tabela_zlaczy[i].liczba_pinow_zlacza;u++)
      { 
        if (tabela_zlaczy[i].mapa_pinow_zlacza[u]!=0)
        {
          UsrLog("\t");
          UsrLog(tabela_zlaczy[i].mapa_pinow_zlacza[u]);
        }
      }
      UsrLogln();
    }
  }
}

void convert2pattern(void)
{
  for (int i = 0;i<128;i++)
  {
    int clean = 0;
    for (int j = 0;j<8;j++)
    {
      if (tabela_polaczen[i][j].numer_zlacza != 0 && tabela_polaczen[i][j].pin != 0)
      {
        uint32_t my_pin = find_pin(tabela_polaczen[i][j].numer_zlacza,tabela_polaczen[i][j].pin);

        tabela_polaczen_pcfs[i][j].pin = my_pin;

        if (j!=0)
        {
          print_k("<->");
        }
        print_k("Z");
        print_k(tabela_polaczen[i][j].numer_zlacza);
        print_k(".");
        print_k(tabela_polaczen[i][j].pin);
        print_k("(");
        print_k(my_pin);
        print_k(")");
        print_k("(");
        print_k(find_pin_positionA(tabela_polaczen_pcfs[i][j].pin)-1);
        print_k(")");
        clean = 1;
      }
    }
   if (clean) print_kln();  
  }

  for (int i = 0;i<128;i++)
  {

    int clean = 0;
    int pin_pattern_pos_maj;
    int pin_pattern_pos_min;

    for (int j = 0;j<8;j++) 
    {
      if (clean) print_k(" ");
      if (tabela_polaczen_pcfs[i][j].pin !=0)
      {
        pin_pattern_pos_maj = find_pin_position(tabela_polaczen_pcfs[i][j].pin);
        print_k(pin_pattern_pos_maj-1);
        //bitClear(pattern.pattern[(pin_pattern_pos_maj*2)][0],pin_pattern_pos_maj);
        clear_pin_position(pin_pattern_pos_maj,pin_pattern_pos_maj);
        print_k(".");
        for (int l = 0; l<8;l++)
        {
          if (l != j)
          {
            if (find_pin_position(tabela_polaczen_pcfs[i][l].pin) != 0)
            {
              pin_pattern_pos_min = find_pin_position(tabela_polaczen_pcfs[i][l].pin);
              print_k(pin_pattern_pos_min-1);
              clear_pin_position(pin_pattern_pos_maj-1,pin_pattern_pos_min-1);
              //bitClear(pattern.pattern[(pin_pattern_pos_maj*2)][0],pin_pattern_pos_min);
              print_k(".");
              clean = 1;
            }
          }
        }
      }
    }
    if (clean) print_kln();  
  }
  //print_k(find_pin_positionA(0));
  print_kln("Pettern generated");
  for (int i=0;i<64;i++)
  {
    print_k(pattern.pattern[(i*2)][int_bank],BIN);
    print_k(" ");
    print_kln(pattern.pattern[(i*2+1)][int_bank],BIN);
  }

}

uint32_t find_pin(uint8_t z, uint8_t p)
{
  for (int i=0;i<16;i++)
  {
    if (tabela_zlaczy[i].numer_zlacza!=0)
    {
      for (int u=0;u<tabela_zlaczy[i].liczba_pinow_zlacza;u++)
      { 
        if (tabela_zlaczy[i].mapa_pinow_zlacza[u]!=0)
        {
          if ((i+1) == z && (u+1) == p)
          {
            //DbgLogln("Found");
            return tabela_zlaczy[i].mapa_pinow_zlacza[u];
          }
        }
      }
      //UsrLogln();
    }
  }
  //DbgLogln("Not Found");
  return 0;
}

void set_bank(int __bank)
{
  int_bank = __bank;   
}

void clear_pin_position(int _pos1, int _pos2)
{
  if (_pos2<32)
  {
    bitClear(pattern.pattern[(_pos1*2)][int_bank],_pos2);
  }
  else
  {
    bitClear(pattern.pattern[(_pos1*2+1)][int_bank],_pos2);
  }
  
}

int find_pin_position(int _pin)
{
  int _tempA = find_pin_positionA(_pin);
  int _tempB = find_pin_positionB(_pin);
  if (_tempA != 0)
  {
    return _tempA;
  }
  else if (_tempB != 0)
  {
    return _tempB;
  }
  else
  {
    return 0;
  }
  return 0;
}

int find_pin_positionA(int _pin)
{
  for (int k = 1;k<33;k++)
  {
    if (_pin == stronaA[k-1])
    {
      return k;
    }
  }
  return 0;
}

int find_pin_positionB(int _pin)
{
  for (int k = 1;k<33;k++)
  {
    if (_pin == stronaB[k-1])
    {
      return k;
    }
  }
  return 0;
}