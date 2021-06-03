

#include <WiFi.h>
#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

typedef JsonDocument json;

#include "thingsiot/iot_enums.hpp"
#include "thingsiot/iot_resource.hpp"


//#include <utility>
//#include <vector>
//#include <algorithm>


class ThingsIoT32 : public msg_enums, public iot_resource {
public:


//    void addResource(const char *str, void (*func)()) {
//        Resource _res;
//        _res.resource_name = str;
//        _res.clbk = func;
//        m_resources.push_back(_res);
//        tmp= "";
//    }



    ThingsIoT32(const char *username, const char *deviceName, const char *devicePassword_) : client(espClient) {
        Username = username;
        DeviceName = deviceName;
        DevicePassword = devicePassword_;

        outname = DeviceName + "@" + Username;

        pub_topic = "dts/" + outname;
        sub_topic = "std/" + outname;


    };

    ~ThingsIoT32() {

    }


    void setWiFi(const char *ssID, const char *ssID_password) {
        ssid = ssID;
        ssid_password = ssID_password;
    }

    void init() {


        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, ssid_password);

        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }

        randomSeed(micros());

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());


        client.setServer(mqttServerHost, mqttServerPort);
        client.setCallback([this](char *topic, byte *payload, unsigned int length) {
            callback(topic, payload, length);
        });

    }

    void handle() {
        if (!client.connected()) {
            while (!client.connected()) {
                Serial.print("Attempting connection...");
                Serial.println(outname);
                if (client.connect(outname.c_str(), outname.c_str(), DevicePassword.c_str())) {
                    Serial.println("connected");
                    client.subscribe(sub_topic.c_str());
                } else {
                    Serial.print("failed, rc=");
                    Serial.print(client.state());
                    Serial.println(" try again in 5 seconds");
                    delay(5000);
                }
            }
        }
        this->client.loop();
    }

    void publishMSG(const char *msg_) {
        client.publish(pub_topic.c_str(), msg_);
    }

    void setHost(const char *host) {
        mqttServerHost = host;
    }

private:


    bool initiated = false;

    const char *ssid = "";
    const char *ssid_password = "";

    const char *mqttServerHost = "10.0.0.150";
    uint16_t mqttServerPort = 1883;


    String Username;
    String DeviceName;
    String DevicePassword;

    String outname;
    String pub_topic;
    String sub_topic;


    WiFiClient espClient;
    PubSubClient client;

    void callback(char *topic, byte *payload, unsigned int length) {

        String incoming_data = F("");
//        Serial.println();
//        Serial.print("Message arrived [");
//        Serial.print(topic);
//        Serial.print("] ");
//        Serial.println();
        for (int i = 0; i < length; i++) {
            incoming_data.concat((char) payload[i]);
        }
//        Serial.print(incoming_data);

        onMessage(incoming_data);
    }


    void onMessage(String &incoming_str) {
        DynamicJsonDocument data_in(1024);
        DynamicJsonDocument data_out(1024);

        deserializeJson(data_in, incoming_str);

        String resp_to_server;

        switch ((uint8_t) data_in["mt"]) {
            case RESOURCE_DESCRIBE: {

                data_out["mi"] = data_in["mi"];
                data_out["mt"] = RESOURCE_RESPONSE;


                fill_api_resources(data_out);

                serializeJson(data_out, resp_to_server);
                publishMSG(resp_to_server.c_str());
            }

                break;

            case RESOURCE_GET_VAL: {

                data_out["mi"] = data_in["mi"];
                data_out["mt"] = RESOURCE_RESPONSE;


                StaticJsonDocument<200> _out;

                run_callback_data_out(data_in, _out);

                if (_out.isNull()) data_out["out"] = nullptr;
                else data_out["out"] = _out;


                serializeJson(data_out, resp_to_server);
                publishMSG(resp_to_server.c_str());

            }
                break;

            case RESOURCE_SET_VAL: {

                data_out["mi"] = data_in["mi"];
                data_out["mt"] = RESOURCE_RESPONSE;


                StaticJsonDocument<4> _out;

                run_callback_data_io(data_in, _out);

//                if (_out.isNull()) data_out["out"] = nullptr;
//                else data_out["out"] = _out;

                data_out["out"] = nullptr;


                serializeJson(data_out, resp_to_server);
                publishMSG(resp_to_server.c_str());

            }
                break;


            default:
                break;

        }


        data_in.garbageCollect();
        data_out.garbageCollect();
    }


};


inline void digital_pin(json &in, json &out, int pin) {

    if (in["val"].isNull()) {
        out["val"] = (bool) digitalRead(pin);
    } else {
        digitalWrite(pin, in["val"]);
    }

}


#define outputValue(value) [](json & out){   out[value] = value;}
#define digitalPin(PIN) [](json & in, json & out){ digital_pin(in, out, PIN); }

