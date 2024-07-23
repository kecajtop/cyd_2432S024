#include "Arduino.h"
#include <SD.h>
#include <Update.h>
#include "macros.h"
#include "simple_tft.h"
#include "sd_fw_upgrade.h"

int sd_new_fw(void)
{
    File firmware =  SD.open("/firmware.bin");
    
    if (firmware) 
    {
      firmware.close();
      delay(2000);
      return 1;
    }
    else
    {
      return 3;
    }
}

void sd_fw_upgrade(void)
{
    Serial.print(F("\n[M] Search for firmware.."));

    File firmware =  SD.open("/firmware.bin");
    
    if (firmware) {
      Serial.println(F("[M] found!"));
      Serial.println(F("[M] Try to update!"));

      display_firmware_update();
 
      Update.onProgress(progressCallBack);
 
      Update.begin(firmware.size(), U_FLASH);
      Update.writeStream(firmware);
      if (Update.end()){
          Serial.println(F("[M] Update finished!"));
          display_firmware_update_ok();
      }else{
          Serial.println(F("[E] Update error!"));
          display_firmware_update_fail();
          Serial.println(Update.getError());
      }
 
      firmware.close();

      SD.remove("/firmware.bak");
 
      if (SD.rename("/firmware.bin", "/firmware.bak")){
          Serial.println(F("[M] Firmware rename succesfully!"));
      }else{
          Serial.println(F("[E] Firmware rename error!"));
      }
      delay(2000);
      display_firmware_update_restart();
      delay(2000);
 
      ESP.restart();
    }
    else
    {
      Serial.println(F("[E] not found!"));
    }
 
  Serial.print(F("[I] Firmware version "));
  Serial.println(FIRMWARE_VERSION);
}

void progressCallBack(size_t currSize, size_t totalSize) {
      Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
      display_firmware_update_progress(currSize, totalSize);
      
}