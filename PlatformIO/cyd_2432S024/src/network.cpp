#include <Wifi.h>
#include <ArduinoOTA.h>
#include "macros.h"
#include "network.h"

#define TIME_ZONE_OFFSET_HRS 1 // UTC+1 for Germany, winter time
#define NTP_UPDATE_INTERVAL_MS 60000L // Update every 60s automatically

wifiConnected_t wifiConnected;

extern const char* ssid;
extern const char* password;
extern uint8_t start_flag;

static bool wifiFirstConnected;

unsigned long last = 0;

int initWiFi(const char *host_name) 
{
  uint8_t i = 0;
  WiFi.disconnect(true);
  WiFi.setHostname(host_name);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.onEvent(WiFiEvent);
  msgln("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    i++;
    delay(100);
    if (i>=100)
    {
      print_kln("Failed");
      return 0;
    }
  }
  print_k(F("\tIP: "));
  print_kln(WiFi.localIP());
  print_k(F("\tRRSI: "));
  print_kln(WiFi.RSSI());
  return 1;
}

void get_network_info(void){
    if(WiFi.status() == WL_CONNECTED) {
        print_k("[*] Network information for ");
        print_kln(ssid);
        print_kln("[+] BSSID : " + WiFi.BSSIDstr());
        print_k("[+] Gateway IP : ");
        print_kln(WiFi.gatewayIP());
        print_k("[+] Subnet Mask : ");
        print_kln(WiFi.subnetMask());
        print_kln((String)"[+] RSSI : " + WiFi.RSSI() + " dB");
        print_k("[+] ESP32 IP : ");
        print_kln(WiFi.localIP());
        Serial.printf("[+] ESP32 Chip model %s Rev %d\n\r", ESP.getChipModel(),
                  ESP.getChipRevision());
        Serial.printf("[+] This chip has %d cores\n\r", ESP.getChipCores());
    }
}

void start_ota(void)
{ 
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      print_kln("Start updating " + type);
    })
    .onEnd([]() {
      print_kln("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");
  
}


void get_task_list()
{
    print_kln("[N] RTOS task list");
    //vTaskList(ptrTaskList);
    Serial.println("**********************************");
    Serial.println("Task  State   Prio    Stack    Num"); 
    Serial.println("**********************************");
    //Serial.print(ptrTaskList);
    Serial.println("**********************************");
    print_kln("Done");
}


void WiFiEvent(WiFiEvent_t event)
{
    //Serial.printf("[WiFi-event] event: %d\n", event);

    switch (event) {
        case ARDUINO_EVENT_WIFI_READY: 
            Serial.println("[E] WiFi interface ready");
            break;
        case ARDUINO_EVENT_WIFI_SCAN_DONE:
            Serial.println("[E] Completed scan for access points");
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("[E] WiFi client started");
            break;
        case ARDUINO_EVENT_WIFI_STA_STOP:
            Serial.println("[E] WiFi clients stopped");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            //Serial.println("[E] Connected to access point");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("[E] Disconnected from WiFi access point");
            //stop_ntp();
            wifiConnected = DISCONNECTED;
            break;
        case ARDUINO_EVENT_WIFI_STA_AUTHMODE_CHANGE:
            Serial.println("[E] Authentication mode of access point has changed");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            //Serial.print("[E] Obtained IP address: ");
            Serial.println(WiFi.localIP());
            //start_ntp();
            wifiConnected = CONNECTED;
            break;
        case ARDUINO_EVENT_WIFI_STA_LOST_IP:
            Serial.println("[E] Lost IP address and IP address is reset to 0");
            break;
        case ARDUINO_EVENT_WPS_ER_SUCCESS:
            Serial.println("[E] WiFi Protected Setup (WPS): succeeded in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_FAILED:
            Serial.println("[E] WiFi Protected Setup (WPS): failed in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_TIMEOUT:
            Serial.println("[E] WiFi Protected Setup (WPS): timeout in enrollee mode");
            break;
        case ARDUINO_EVENT_WPS_ER_PIN:
            Serial.println("[E] WiFi Protected Setup (WPS): pin code in enrollee mode");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("[E] WiFi access point started");
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("[E] WiFi access point stopped");
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("[E] Client connected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("[E]Client disconnected");
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            Serial.println("[E] Assigned IP address to client");
            break;
        case ARDUINO_EVENT_WIFI_AP_PROBEREQRECVED:
            Serial.println("[E] Received probe request");
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            Serial.println("[E] AP IPv6 is preferred");
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            Serial.println("[E] STA IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP6:
            Serial.println("[E] Ethernet IPv6 is preferred");
            break;
        case ARDUINO_EVENT_ETH_START:
            Serial.println("[E] Ethernet started");
            break;
        case ARDUINO_EVENT_ETH_STOP:
            Serial.println("[E] Ethernet stopped");
            break;
        case ARDUINO_EVENT_ETH_CONNECTED:
            Serial.println("[E] Ethernet connected");
            break;
        case ARDUINO_EVENT_ETH_DISCONNECTED:
            Serial.println("[E] Ethernet disconnected");
            break;
        case ARDUINO_EVENT_ETH_GOT_IP:
            Serial.println("[E] Obtained IP address");
            break;
        default: break;
    }
}