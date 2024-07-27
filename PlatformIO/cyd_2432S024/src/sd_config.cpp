#include "Arduino.h"
#include <SD.h>
#include <Update.h>
#include "macros.h"
#include "sd_config.h"
#include "sd_card_csv.h"
#include <CSV_Parser.h>

const char * f_name_z = "/10040237z.csv";
const char * f_name_p = "/10040237p.csv";
//const int chipSelect = 10;

tabela_zlaczy_t tabela_zlaczy[16];
//{
//  1,6,"Z1",
//  {
//    3,5,7,2,4,6
//  },
//  2,4,"Z2",
//  {
//    8,9,10,12,0,0
//  }
//};

tabela_polaczen_zlaczy_t tabela_polaczen[128][8];
//{
//  {{1,1},{2,1},{0,0},{0,0}},
//  {{1,3},{2,2},{0,0},{0,0}},
//  {{0,0},{0,0},{0,0},{0,0}},
//  {{1,6},{2,4},{1,5},{2,3}},
//  {{1,6},{2,4},{1,5},{2,3}}
//};

tabela_polaczen_pcfs_t tabela_polaczen_pcfs[128][8];

File file;
char feedRowParser() {
  return file.read();
}

bool rowParserFinished() {
  return ((file.available()>0)?false:true);
}

/// @brief 
/// @param f_name 
int load_csv(const char * f_name)
{
    //f_name_z = "/" + String(f_name) + "z.csv"; 
    //f_name_p = "/" + String(f_name) + "p.csv";

    clear_tables();


    if (!SD.exists(f_name_p)) 
    {
      Serial.println("ERROR: File \"" + String(f_name_p) + "\" does not exist.");
      return 0;  
      //while (1);
    }
    else
    {
      file = SD.open(f_name_p, FILE_READ);
      if (!file) 
        {
          Serial.println("ERROR: File open failed");
          //while (1);
          return 0;
        }
      
      Serial.println("File \"" + String(f_name_p) + "\" loaded"); 

      CSV_Parser cp(/*format*/ "Ldddddddddddddddd");

      int row_index = 0;

      uint32_t *ids = (uint32_t*)cp[0];
      uint16_t *zloncze1s = (uint16_t*)cp[1];
      uint16_t *pin1s = (uint16_t*)cp[2];
      uint16_t *zloncze2s = (uint16_t*)cp[3];
      uint16_t *pin2s = (uint16_t*)cp[4];
      uint16_t *zloncze3s = (uint16_t*)cp[5];
      uint16_t *pin3s = (uint16_t*)cp[6];
      uint16_t *zloncze4s = (uint16_t*)cp[7];
      uint16_t *pin4s = (uint16_t*)cp[8];
      uint16_t *zloncze5s = (uint16_t*)cp[9];
      uint16_t *pin5s = (uint16_t*)cp[10];
      uint16_t *zloncze6s = (uint16_t*)cp[11];
      uint16_t *pin6s = (uint16_t*)cp[12];
      uint16_t *zloncze7s = (uint16_t*)cp[13];
      uint16_t *pin7s = (uint16_t*)cp[14];
      uint16_t *zloncze8s = (uint16_t*)cp[15];
      uint16_t *pin8s = (uint16_t*)cp[16];

      
      

      while (cp.parseRow()) 
      {
        uint16_t id = ids[0];

        if (id == 0)
        {
          uint32_t _patternA = 0;
          uint32_t _patternB = 0;

          uint16_t _pcf1 = zloncze1s[0];
          uint16_t _pcf2 = pin1s[0];
          uint16_t _pcf3 = zloncze2s[0];
          uint16_t _pcf4 = pin2s[0];

          if (_pcf1 == 1)
          {
            _patternA = 0x000000FF;
          }
          if (_pcf2 == 1)
          {
            _patternA = _patternA + 0x0000FF00;
          }
          if (_pcf3 == 1)
          {
            _patternA = _patternA + 0x00FF0000;
          }
          if (_pcf4 == 1)
          {
            _patternA = _patternA + 0xFF000000;
          }
        
          uint16_t _pcf5 = zloncze3s[0];
          uint16_t _pcf6 = pin3s[0];
          uint16_t _pcf7 = zloncze4s[0];
          uint16_t _pcf8 = pin4s[0];
          
          if (_pcf5 == 1)
          {
            _patternB = 0x000000FF;
          }
          if (_pcf6 == 1)
          {
            _patternB = _patternB + 0x0000FF00;
          }
          if (_pcf7 == 1)
          {
            _patternB = _patternB + 0x00FF0000;
          }
          if (_pcf8 == 1)
          {
            _patternB = _patternB +0xFF000000;
          }
          
          
          clear_table_with_pattern(_patternA,_patternB);
        }
        else
        {  
          //uint16_t id = ids[0];
          uint16_t _index = row_index - 1;
          uint16_t zloncze1 = tabela_polaczen[_index][0].numer_zlacza = zloncze1s[0];
          uint16_t pin1 = tabela_polaczen[_index][0].pin = pin1s[0];
          uint16_t zloncze2 = tabela_polaczen[_index][1].numer_zlacza = zloncze2s[0];
          uint16_t pin2 = tabela_polaczen[_index][1].pin = pin2s[0];
          uint16_t zloncze3 = tabela_polaczen[_index][2].numer_zlacza = zloncze3s[0];
          uint16_t pin3 = tabela_polaczen[_index][2].pin = pin3s[0];
          uint16_t zloncze4 = tabela_polaczen[_index][3].numer_zlacza = zloncze4s[0];
          uint16_t pin4 = tabela_polaczen[_index][3].pin = pin4s[0];
          uint16_t zloncze5 = tabela_polaczen[_index][4].numer_zlacza = zloncze5s[0];
          uint16_t pin5 = tabela_polaczen[_index][4].pin = pin5s[0];
          uint16_t zloncze6 = tabela_polaczen[_index][5].numer_zlacza = zloncze6s[0];
          uint16_t pin6 = tabela_polaczen[_index][5].pin = pin6s[0];
          uint16_t zloncze7 = tabela_polaczen[_index][6].numer_zlacza = zloncze7s[0];
          uint16_t pin7 = tabela_polaczen[_index][6].pin = pin7s[0];
          uint16_t zloncze8 = tabela_polaczen[_index][7].numer_zlacza = zloncze8s[0];
          uint16_t pin8 = tabela_polaczen[_index][7].pin = pin8s[0];


          if (zloncze1!=0)
          {
            Serial.print("id=");
            Serial.print(id, DEC);
            if (zloncze1!=0)
            {
              Serial.print(", Z");
              Serial.print(zloncze1);
              Serial.print(".");
              Serial.print(pin1);
            }
            if (zloncze2!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze2);
              Serial.print(".");
              Serial.print(pin2);
            }
            if (zloncze3!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze3);
              Serial.print(".");
              Serial.print(pin3);
            }
            if (zloncze4!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze4);
              Serial.print(".");
              Serial.print(pin4);
            }
            if (zloncze5!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze5);
              Serial.print(".");
              Serial.print(pin5);
            }
            if (zloncze6!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze6);
              Serial.print(".");
              Serial.print(pin6);
            }
            if (zloncze7!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze7);
              Serial.print(".");
              Serial.print(pin7);
            }
            if (zloncze8!=0)
            {
              Serial.print("<->Z");
              Serial.print(zloncze8);
              Serial.print(".");
              Serial.print(pin8);
            }
            Serial.println();
          }
        }
      row_index++;
      }
      file.close();
    }

    if (!SD.exists(f_name_z)) 
    {
      Serial.println("ERROR: File \"" + String(f_name_z) + "\" does not exist.");  
      //while (1);
      return 0;
    }
    else
    {
      file = SD.open(f_name_z, FILE_READ);
      if (!file) 
        {
          Serial.println("ERROR: File open failed");
          //while (1);
          return 0;
        }

      Serial.println("File \"" + String(f_name_z) + "\" loaded"); 

      CSV_Parser cp(/*format*/ "Ldcddd");

      // parseRow calls feedRowParser() continuously until it reads a 
      // full row or until the rowParserFinished() returns true
      int row_index = 0;

      // WARNING: String indexing can't be used here because the header was not supplied to the cp object yet.
      // int32_t *ids = (int32_t*)cp["Index"];
      // char **customer_ids = (char**)cp["Customer Id"];
      // char **emails = (char**)cp["Email"];

      
      uint32_t *ids = (uint32_t*)cp[0];
      uint16_t *zlonczes = (uint16_t*)cp[1];
      char **nazwa = (char**)cp[2];
      uint16_t *piny = (uint16_t*)cp[3];
      uint16_t *out_of = (uint16_t*)cp[4];
      uint16_t *pcfs = (uint16_t*)cp[5];

      //tabela_zlaczy.nazwa_zlacza = 0;//(char**)cp[2];
      //tabela_zlaczy->numer_zlacza = cp[1];
      //char **customer_ids = (char**)cp[2];
      //char **emails = (char**)cp[9];

      while (cp.parseRow()) {
      // Here we could use string indexing but it would be much slower.
      // char *email = ((char**)cp["Email"])[0];

      uint16_t id = ids[0];
      uint16_t zloncze = tabela_zlaczy[zlonczes[0]-1].numer_zlacza = zlonczes[0];
      uint16_t pin = tabela_zlaczy[zlonczes[0]-1].mapa_pinow_zlacza[piny[0]-1] = pcfs[0];
      tabela_zlaczy[zlonczes[0]-1].liczba_pinow_zlacza = out_of[0];
      //int32_t pin1 = tabela_polaczen[row_index][0].pin = pin1s[0];
      //int32_t zloncze2 = tabela_polaczen[row_index][1].numer_zlacza = zloncze2s[0];
      //int32_t pin2 = tabela_polaczen[row_index][1].pin = pin2s[0];
      //int32_t zloncze3 = tabela_polaczen[row_index][2].numer_zlacza = zloncze3s[0];
      //int32_t pin3 = tabela_polaczen[row_index][2].pin = pin3s[0];
      //int32_t zloncze4 = tabela_polaczen[row_index][3].numer_zlacza = zloncze4s[0];
      //int32_t pin4 = tabela_polaczen[row_index][3].pin = pin4s[0];
      //int32_t zloncze5 = tabela_polaczen[row_index][4].numer_zlacza = zloncze5s[0];
      //int32_t pin5 = tabela_polaczen[row_index][4].pin = pin5s[0];
      //char *customer_id = customer_ids[0];
      //char *email = emails[0];

      //Serial.print(String(row_index) + ". customer_id=" + String(customer_id));
      Serial.print("id=");
      Serial.print(id, DEC);
      Serial.print(", Z");
      Serial.print(zloncze);
      Serial.print(".");
      Serial.print(piny[0]);
      Serial.print("(");
      Serial.print(pcfs[0]);
      Serial.print(")");
      Serial.println();
      row_index++;
      }
      file.close();
    }

    convert2pattern();
    
    //while(1)
    //{
      //syslog.printf(FAC_USER, PRI_WARNING, "Looping around... Uptime: %0.1f minutes", millis() / (float)60000);
      //tft.setTextColor(TFT_BLACK);
      //tft.setCursor (12, 5);
      //tft.print("Original ADAfruit font!");
      //tft.setTextColor(TFT_BLACK, TFT_BLACK); // Do not plot the background colour
      //tft.drawCentreString("12.34", 80, 54, 6);
      //delay(1000);
      //lv_timer_handler(); /* let the GUI do its work */
      //delay(5); /* let this time pass */  
      //delay(1000);
    //}
  return 1;
}