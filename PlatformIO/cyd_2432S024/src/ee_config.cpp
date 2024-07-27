#include "Arduino.h"
#include <EEPROM.h>
#include "I2C_eeprom.h"
#include "config.h"
#include "macros.h"
#include "ee_config.h"

I2C_eeprom een(i2c_address, I2C_DEVICESIZE_24LC256);

extern pattern_struct_t pattern;

void ee_init()
{
    een.begin();
}

int ee_info()
{
    //Serial.println("Please Wait this may take a while depending upon you EEPROM size.");
   // Serial.print("Size of EEPROM : ");
    //Serial.print(ee.length());
    //Serial.println(" KB");

    if (! een.isConnected())
    {
      print_kln("[E] Can't find eeprom (stopped)...");
        return 0;
      // while (1);
    }
    else
    {
      print_kln("\nDetermine size");
      uint32_t size = een.determineSize(false);  //  debug parameter
      if (size == 0)
      {
        print_kln("SIZE: could not determine size");
      }
      else if (size > 1024)
      {
        print_k("SIZE: ");
        print_k(size / 1024);
        print_kln(" KB.");
      }
      else
      {
        print_k("SIZE: ");
        print_k(size);
        print_kln(" bytes.");
      }

      print_k("PAGE: ");
      uint8_t pageSize = een.getPageSize(size);
      print_k(pageSize);
      print_kln(" bytes.");
      print_kln("");
      return 1;
    }
    return 0;
}

void dumpEEPROM(uint16_t memoryAddress, uint16_t length)
{
  const int BLOCK_TO_LENGTH = 10;

  print_k("\t  ");
  for (int x = 0; x < 10; x++)
  {
    if (x != 0) print_k("    ");
    print_k(x);
  }
  print_kln();

  // block to defined length
  memoryAddress = memoryAddress / BLOCK_TO_LENGTH * BLOCK_TO_LENGTH;
  length = (length + BLOCK_TO_LENGTH - 1) / BLOCK_TO_LENGTH * BLOCK_TO_LENGTH;

  byte b = een.readByte(memoryAddress);
  for (unsigned int i = 0; i < length; i++)
  {
    char buf[6];
    if (memoryAddress % BLOCK_TO_LENGTH == 0)
    {
      if (i != 0) print_kln();
      sprintf(buf, "%05x", memoryAddress);
      print_k(buf);
      print_k(":\t");
    }
    sprintf(buf, "%02x", b);
    print_k(buf);
    b = een.readByte(++memoryAddress);
    print_k("   ");
  }
 print_kln();
}

uint32_t erase_mode()
{
  print_kln("Erase mode");
  if (een.setBlockVerify(_CONFIG_START, 255, sizeof(pattern)) == false)
  {
      print_kln("Fail");
      return 0;
  }
  print_kln("OK");
  return 1;
}

uint32_t load_pattern()
{
  UsrLogln("Loading pattern from EEPROM ... ");
  // To make sure there are settings, and they are YOURS!
  //UsrLogln(ee.read(_CONFIG_START + sizeof(pattern.pattern) + 0),HEX);
  //UsrLogln(pattern.version_of_program[0],HEX);
  // If nothing is found it will use the default settings.
  if (//EEPROM.read(CONFIG_START + sizeof(settings) - 1) == settings.version_of_program[3] // this is '\0'
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 8) == pattern.version_of_program[8] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 7) == pattern.version_of_program[7] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 6) == pattern.version_of_program[6] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 5) == pattern.version_of_program[5] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 4) == pattern.version_of_program[4] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 3) == pattern.version_of_program[3] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 2) == pattern.version_of_program[2] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 1) == pattern.version_of_program[1] &&
      een.readByte(_CONFIG_START + sizeof(pattern.pattern) + 0) == pattern.version_of_program[0])
  { // reads settings from EEPROM
    for (uint16_t t=0; t<sizeof(pattern); t++)
      *((byte*)&pattern + t) = een.readByte(_CONFIG_START + t);
        print_kln("OK");
        return 1;
      
  } else {
    print_kln("Memory empty ...");
    return 0;
  }
  return 0;
}

uint32_t save_pattern()
{
  UsrLogln("Saving pattern to EEPROM ... ");
  //UsrLogln(een.writeBlock(_CONFIG_START, (uint8_t *) &pattern, sizeof(pattern)));
  //if (een.writeBlockVerify(_CONFIG_START, (uint8_t *) &pattern, sizeof(pattern)) == false);
  //{
  //    UsrLogln("Fail");
  //    return 0;
  //}
  for (uint16_t t=0; t<sizeof(pattern); t++)
  { // writes to EEPROM
    een.writeByte(_CONFIG_START + t, *((byte*)&pattern + t));
    // and verifies the data
  }
  //EEPROM.commit();
  delay(5);
  for (uint16_t t=0; t<sizeof(pattern); t++)
  { // writes to EEPROM
    if (een.readByte(_CONFIG_START + t) != *((byte*)&pattern + t))
    {
      // error writing to EEPROM
      print_k("Fail 0x");
      print_k(t,HEX);
      print_k(" address ERROR Check with value 0x");
      print_k(een.readByte(_CONFIG_START + t));
      dumpEEPROM(_CONFIG_START,sizeof(pattern));
      return 0;
    }
  }
  print_kln("OK");
  return 1;
}