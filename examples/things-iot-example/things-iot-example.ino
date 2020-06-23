#include <ESP8266WiFi.h>
#include <things8266.h>


// Boilerplate code 
//------------------------
String HandleDataResponse(String res, String type, String value);
String Things8266::HandleData(String res, String type, String value) {
    return HandleDataResponse(res, type, value);
}
Things8266 DeviceClient;
WiFiClient espClient;
PubSubClient client(espClient);
//--------------------------




int LED = 2;

String testString;
String placeholderString = "this is string";



void setup()
{

    Serial.begin(115200);
    DynamicJsonDocument thing(1024);
    thing["action"] = "create_resources";



    // Set Output Pins

    pinMode(LED, OUTPUT);

    //---------------


    // SetUp Device

    DeviceClient.SetHost("things-iot.siddhesh.me");

    DeviceClient.SetDevice("username", "device-id", "device-pass");

    DeviceClient.SetWiFi("ssid", "ssid-pass");




    // CREATE RESOURCES HERE


    // Kind: switch
    // Type: input-output we can view it and modify it from cloud

    thing["resources"]["led"]["type"] = "io";
    thing["resources"]["led"]["kind"] = "switch";


    // Kind: String / Value ( 2 way )
    // Type: input-output we can view it and modify it from cloud

    thing["resources"]["placeholder"]["type"] = "io";
    thing["resources"]["placeholder"]["kind"] = "value";


    // Kind: String / Value ( 1 way )
    // Type Output Only! (we cannot modify it from cloud) [only view it from cloud]

    thing["resources"]["temp"]["type"] = "out";
    thing["resources"]["temp"]["kind"] = "value";



    DeviceClient.initDevice(client, thing);
}



String HandleDataResponse(String res, String type, String value)
{

    if (res == "led") return DeviceClient.digitalPin(LED, type, value);


    if (res == "temp") return DeviceClient.outputValue(testString);


    if (res == "placeholder") return DeviceClient.inputValue(placeholderString, type, value);

}



void loop()
{
    testString = String(millis());

    DeviceClient.DataHandle();
}
