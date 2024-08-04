#include "Arduino.h"
#include <SD.h>
#include <Update.h>
#include "macros.h"
#include "simple_tft.h"
#include "fw_upgrade.h"
#include "config.h"
#include "ESP32OTAPull.h"

ESP32OTAPull ota;

int sd_new_fw(void)
{
    File firmware =  SD.open(_SD_FW_NAME_ESP32);
    
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

    File firmware =  SD.open(_SD_FW_NAME_ESP32);
    
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

      SD.remove(_SD_FW_NAME_ESP32_BAK);
 
      if (SD.rename(_SD_FW_NAME_ESP32, _SD_FW_NAME_ESP32_BAK)){
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
 
  Serial.print(F("[I] Firmware version: "));
  Serial.println(_FW_VERSION_ESP32);
}

void progressCallBack(size_t currSize, size_t totalSize) 
{
    Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
    display_firmware_update_progress(currSize, totalSize);
}

String online_fw_check(void)
{
  //See if an update is available (but don't do it).  No callback routine
	Serial.printf("Check for update, but don't download it.\n");
  Serial.printf(JSON_URL);
	int ret = ota.CheckForOTAUpdate(JSON_URL, _FW_VERSION_ESP32, ESP32OTAPull::DONT_DO_UPDATE);
	Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

	// After we've checked, we can obtain the version from the JSON file
  String otaVersion = ota.GetVersion();
	Serial.printf("OTA Version Available: %s\n", otaVersion.c_str());
  display_selftest_msg("OTA FW.", otaVersion.c_str());
  return otaVersion;
}

	// Example 2
	//Serial.printf("Check for update and download it, but don't reboot.  Display dots.\n");
	//ret = ota
	//	.SetCallback(callback_dots)
	//	.CheckForOTAUpdate(JSON_URL, _FW_VERSION, ESP32OTAPull::UPDATE_BUT_NO_BOOT);
	//Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

	//delay(3000);

	// Example 3
	//Serial.printf("Download and install downgrade, but only if the configuration string matches.  Display percentages.\n");
	//ret = ota
	//	.SetCallback(callback_percent)
	//	.AllowDowngrades(true)
	//	.SetConfig("4MB RAM")
	//	.CheckForOTAUpdate(JSON_URL, "99.99.99", ESP32OTAPull::UPDATE_AND_BOOT);
	//Serial.printf("CheckForOTAUpdate returned %d (%s)\n\n", ret, errtext(ret));

const char *errtext(int code)
{
	switch(code)
	{
		case ESP32OTAPull::UPDATE_AVAILABLE:
			return "An update is available but wasn't installed";
		case ESP32OTAPull::NO_UPDATE_PROFILE_FOUND:
			return "No profile matches";
		case ESP32OTAPull::NO_UPDATE_AVAILABLE:
			return "Profile matched, but update not applicable";
		case ESP32OTAPull::UPDATE_OK:
			return "An update was done, but no reboot";
		case ESP32OTAPull::HTTP_FAILED:
			return "HTTP GET failure";
		case ESP32OTAPull::WRITE_ERROR:
			return "Write error";
		case ESP32OTAPull::JSON_PROBLEM:
			return "Invalid JSON";
		case ESP32OTAPull::OTA_UPDATE_FAIL:
			return "Update fail (no OTA partition?)";
		default:
			if (code > 0)
				return "Unexpected HTTP response code";
			break;
	}
	return "Unknown error";
}

void callback_percent(int offset, int totallength)
{
	static int prev_percent = -1;
	int percent = 100 * offset / totallength;
	if (percent != prev_percent)
	{
		Serial.printf("Updating %d of %d (%02d%%)...\n", offset, totallength, 100 * offset / totallength);
    display_firmware_update_progress(offset, totallength);
		prev_percent = percent;
	}
}

void callback_dots(int offset, int totallength)
{
	Serial.print(".");
}