#include <ESP8266WiFi.h>
#include "PubSubClient/PubSubClient.h"
#include "ArduinoJson.h"

String HandleDataResponse(String res, String type, String value);

class Things8266 {
  public:
  /*
    Things8266(){
      WiFiClient espClient;
      PubSubClient client(espClient);
      this->client;
    };
    */


    void logic(String data) {
      DynamicJsonDocument data_in(1024);
      DynamicJsonDocument data_out(1024);
      deserializeJson(data_in, data);

      if ((data_in["query"]["action"] == "in") || (data_in["query"]["action"] == "out")) {
        data_out["msg_id"] = data_in["msg_id"];
        data_out["action"] = "returned_api_response";
        data_out["data"] = HandleDataResponse(data_in["resource"], data_in["query"]["action"], data_in["query"]["value"]);
        String outdata;
        serializeJson(data_out, outdata);
        publishMSG(outdata.c_str());
      }

    };

    void SetDevice(String username, String deviceName, String password) {
      this->Username = username;
      this->DeviceName = deviceName;
      this->Password = password;
    }
    void SetWiFi(const char* ssID, const char* ssID_password) {
      this->ssid = ssID;
      this->ssid_password = ssID_password;
    }

    void initDevice(PubSubClient client, DynamicJsonDocument res) {
      res["action"] = "create_resources";
      this->client = client;
      this->topic = this->DeviceName + "/" + this->Username;
      serializeJson(res, this->initJson);
      this->char_initJson = this->initJson.c_str();

      this->outname = this->DeviceName + "@" + this->Username;
      this->char_DeviceName = this->DeviceName.c_str();
      this->char_Password = this->Password.c_str();
      this->char_outname = this->outname.c_str();
      this->char_topic = this->topic.c_str();
      this->initiated = true;
      this->setupIT();

    }

    void DataHandle() {

      if (!client.connected()) {
        while (!client.connected()) {
          Serial.print("Attempting connection...");
          if (client.connect(this->char_outname, this->char_outname, this->char_Password)) {
            Serial.println("connected");
            Serial.println(char_initJson);
            client.subscribe(this->char_topic);
            client.publish(this->char_topic, this->char_initJson);
          } else {
            Serial.print("failed, rc=");
            Serial.print(this->client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
          }
        }
      }
      this->client.loop();

    }

    void publishMSG (const char* info) {
      client.publish(this->char_topic, info);

    }



    String digitalPin(int pin, String type, String value) {
      if (type == "out") {
        digitalWrite(pin, value.toInt());
      }
      return "{ \"value\" : \"" +  String(!!digitalRead(pin)) + "\"  }";
    }


    String outputValue(String globalVar) {
      return "{ \"value\" : \"" + globalVar + "\"  }";
    }


    String inputValue(String &globalVar, String type, String value) {
      if (type == "out") {
        globalVar = String(value);
      }
      return "{ \"value\" : \"" + globalVar + "\"  }";
    }


    void SetHost(const char* host) {
      this->mqttServer = host;
    }

    void CreateResource(DynamicJsonDocument &databj, const char* typ, const char* resource)
    {
      if (typ == "switch") {
        databj["resources"][resource]["type"] = "io";
        databj["resources"][resource]["kind"] = "switch";
      }
      if (typ == "2-way") {
        databj["resources"][resource]["type"] = "io";
        databj["resources"][resource]["kind"] = "value";

      }
      if (typ == "1-way") {

        databj["resources"][resource]["type"] = "out";
        databj["resources"][resource]["kind"] = "value";

      }

    }





  private:
    String Username;
    String DeviceName;
    String Password;

    bool initiated = false;

    const char* ssid;
    const char* ssid_password;

    const char* mqttServer;
    int mqttPort = 1883;

    String topic;
    String outname;
    const char* char_DeviceName;
    const char* char_Password;
    const char* char_outname;
    const char* char_topic;
    String initJson;
    const char* char_initJson;
    PubSubClient client;




    void callback(char* topic, byte* payload, unsigned int length) {
      String srr;
      Serial.println();
      Serial.print("Message arrived [");
      Serial.print(topic);
      Serial.print("] ");
      Serial.println();
      for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        srr.concat((char)payload[i]);
      }
      this->logic(srr);
    }


    void setupIT() {

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

      this->client.setServer(this->mqttServer, this->mqttPort);
      this->client.setCallback([this] (char* topic, byte * payload, unsigned int length) {
        this->callback(topic, payload, length);
      });

    }



};
