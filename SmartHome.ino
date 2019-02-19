#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "WiFiManager.h"  
#include <TimeLib.h> 

#define pinDHT 4
#define DHTTYPE DHT22

char ssid[] = "JmenoWifi"; 
char password[] = "HesloWifi"; 

#define ORG "mr7vcy"
#define DEVICE_TYPE "dht"
#define DEVICE_ID "dht22"
#define TOKEN "acIQqq9jJvhhHPA0KQ"

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
String topicName = String("iot-2/evt/status/fmt/json");
const char cmdTopic[] = "iot-2/cmd/led/fmt/json";
int dataDelay = 2000;
int delayTime;
int myTime = millis()/1000;
int myLastTime = myTime;

//int port = 1883;



WiFiClient wifiClient;
void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String value = "";
  for (int i = 0; i < payloadLength; i++) {
    Serial.print((char)payload[i]);
    value += (char)payload[i];
  }
  int inPayload = value.toInt();

  Serial.println();
  //dataDelay = 2000;
   if (inPayload == 2){
    //delay(10000);
    dataDelay = 10000;
    delayTime = 2;
  }
  else{
    //delay(2000);
    dataDelay = 2000;
    delayTime = 10;
  }
}
PubSubClient client(server, 1883, callback, wifiClient);

int publishInterval = dataDelay;
long lastPublishMillis;

DHT mojeDHT(pinDHT, DHTTYPE);
void setup()
{
  Serial.begin(115200);

  
  //wifi a mqtt
  wifiConnect();
  mqttConnect();
  delay(10);

  // Connect to Wi-Fi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to...");
  Serial.println(ssid);

  DHT mojeDHT(pinDHT, DHTTYPE);
  mojeDHT.begin();
}

void loop () {
  int loopTime = second();
  int x1;
  int x2;
  char topicStr[26];
  topicName.toCharArray(topicStr,26);
    if (millis() - lastPublishMillis > publishInterval) {
    //publishData();
    lastPublishMillis = millis();
  }

  if (!client.loop()) {
    mqttConnect();
  }
  myTime = millis()/1000;
  if (dataDelay == 2000){
  if (myTime - myLastTime >= 2){
Serial.println(myTime);
     String json = buildJson();
   char jsonStr[200];
   json.toCharArray(jsonStr,200);  
 // Publish the data.
      boolean pubresult = client.publish(topicStr,jsonStr);
      Serial.print("attempt to send ");
      Serial.println(jsonStr);
      Serial.print("to ");
      Serial.println(topicStr);
      if (pubresult)
        Serial.println("successfully sent");
      else
        Serial.println("unsuccessfully sent"); 
        myLastTime = myTime;
  }
  }
    if (dataDelay == 10000){
  if (myTime - myLastTime >= 10){
    Serial.println(myTime);
     String json = buildJson();
   char jsonStr[200];
   json.toCharArray(jsonStr,200);  
 // Publish the data.
      boolean pubresult = client.publish(topicStr,jsonStr);
      Serial.print("attempt to send ");
      Serial.println(jsonStr);
      Serial.print("to ");
      Serial.println(topicStr);
      if (pubresult)
        Serial.println("successfully sent");
      else
        Serial.println("unsuccessfully sent"); 
        myLastTime = myTime;
  }
  }
  client.loop();
  
  }
  void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
     if (client.subscribe(cmdTopic)) {
      Serial.println("subscribe to responses OK");
    } else {
      Serial.println("subscribe to responses FAILED");
    }
    Serial.println();
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());

}
void mqttConnect() {
  if (!!!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
      if (client.subscribe(cmdTopic)) {
      Serial.println("subscribe to responses OK");
    } else {
      Serial.println("subscribe to responses FAILED");
    }
    Serial.println();
  }
}
//JSON (Temperature, Humidity, Delay)
String buildJson() {
  int chk = mojeDHT.readTemperature();
  float chum = mojeDHT.readHumidity();
  String payload = "{";
  payload+="\n";
  payload+= "\"d\": {";
  payload+="\n";
  payload+="\"temperature\": " ;
  payload+= chk;
  payload+=",";
  payload+="\"humidity\": ";
  payload+= chum;
  payload+=",";
  payload+="\"delay\": ";
  payload+= dataDelay;
  payload+="\n";
  payload+="}";
  payload+="\n";
  payload+="}";
  return payload;
}

