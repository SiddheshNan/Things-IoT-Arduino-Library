#ifndef IOT_ESP32_H
#define IOT_ESP32_H

#include <WiFiClientSecure.h>
#include "ThingsiotWiFi.h"

#ifndef _DISABLE_TLS_
    class ThingsiotESP32 : public ThingsiotWifiClient<WiFiClientSecure>{
#else
    class ThingsiotESP32 : public ThingsiotWifiClient<WiFiClient>{
#endif

public:
    ThingsiotESP32(const char* user, const char* device, const char* device_credential) :
            ThingsiotWifiClient(user, device, device_credential)
    {}

    ~ThingsiotESP32(){

    }
};

#endif