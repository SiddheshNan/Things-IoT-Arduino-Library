#include <ESP8266WiFi.h>
#include <things8266.h>

WiFiClient espClient;
PubSubClient client(espClient);
Things8266 DeviceClient;


int LED = 2;

String testString;
String placeholderString = "this is string";

void setup()
{

  Serial.begin(115200);
  DynamicJsonDocument thing(1024);



  // Set Output Pins

  pinMode(LED, OUTPUT);


  // SetUp Device

  DeviceClient.SetHost("things-iot.siddhesh.me");

  DeviceClient.SetDevice("username", "device-id", "device-pass");

  DeviceClient.SetWiFi("ssid", "ssid-pass");


  // CREATE RESOURCES HERE

  // Kind: switch
  // Type: input-output we can view it and modify it from cloud

  DeviceClient.CreateResource(thing, "switch", "led");

  // Kind: String / Value ( 2 way )
  // Type: input-output we can view it and modify it from cloud

  DeviceClient.CreateResource(thing, "1-way", "temp");


  // Kind: String / Value ( 1 way )
  // Type Output Only! (we cannot modify it from cloud) [only view it from cloud]

  DeviceClient.CreateResource(thing, "2-way", "placeholder");


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
