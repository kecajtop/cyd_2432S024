#ifndef network_h
#define network_h

#include "Arduino.h"
#include <WiFi.h>

typedef enum wifiConnected 
    {
        IDLE, 
        CONNECTED, 
        DISCONNECTED
    }wifiConnected_t;

int initWiFi(const char * host_name);

void get_network_info(void);

void WiFiEvent(WiFiEvent_t event);

void get_task_list(void);

#endif
