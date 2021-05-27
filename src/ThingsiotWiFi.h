
#ifndef IOT_WIFI_H
#define IOT_WIFI_H

#include "ThingsiotClient.h"

template <class Client>
class ThingsiotWifiClient  : public ThingsiotClient {

public:
    ThingsiotWifiClient(const char* user, const char* device, const char* device_credential) :
            ThingsiotClient(client_, user, device, device_credential),
            wifi_ssid_(NULL),
            wifi_password_(NULL)
    {}

    ~ThingsiotWifiClient(){

    }

protected:

    virtual bool network_connected(){
        return (WiFi.status() == WL_CONNECTED) && !(WiFi.localIP() == (IPAddress)INADDR_NONE);
    }

    virtual bool connect_network(){
        if(wifi_ssid_==NULL){
            _DEBUG("NETWORK", "Cannot connect to WiFi. SSID not set!");
        }

        unsigned long wifi_timeout = millis();
        _DEBUG_VALUE("NETWORK", "Connecting to network ", wifi_ssid_);

        if(wifi_password_!=NULL){
            WiFi.begin((char*)wifi_ssid_, (char*) wifi_password_);
        }else{
            WiFi.begin((char*)wifi_ssid_);
        }

        while( WiFi.status() != WL_CONNECTED) {
            if(millis() - wifi_timeout > 30000) return false;
#ifdef ESP8266
            yield();
#endif
        }
        _DEBUG("NETWORK", "Connected to WiFi!");
        wifi_timeout = millis();
        _DEBUG("NETWORK", "Getting IP Address...");
        while (WiFi.localIP() == (IPAddress)INADDR_NONE) {
            if(millis() - wifi_timeout > 30000) return false;
#ifdef ESP8266
            yield();
#endif
        }
        _DEBUG_VALUE("NETWORK", "Got IP Address: ", WiFi.localIP());
        return true;
    }

public:

    void setWiFi(const char* ssid, const char* password=NULL)
    {
        wifi_ssid_ = ssid;
        wifi_password_ = password;
    }

protected:
    Client client_;
    const char* wifi_ssid_;
    const char* wifi_password_;
};

#define ThingsiotWifi ThingsiotWifiClient<WiFiClient>

#endif