#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

//Wifi configuration
const char* ssid="YOUR_SSID";
const char* password = "YOUR_NETWORK_PASSWORD";

//MQTT configuration
const char* mqttServer = "192.168.43.127";
const int mqttPort = 12948;
const char* mqttUser = "smartHomeGateway";
const char* mqttPassword = "1234Isep";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void connectToMqtt();
void publishMessage(char* JSONmessageBuffer);
void receiveMessage(char* topic, byte* payload, unsigned int length);
void connectToNetwork();
void getData(char *allpayload,char * data);
int strpos(char* string, char* substring);

//Setup
void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  connectToNetwork();
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(receiveMessage);
  connectToMqtt();
  mqttClient.subscribe("setFanState");
  mqttClient.subscribe("setThreshold");
}

void loop() {
  connectToNetwork();
  connectToMqtt();
  if(Serial.available())
    publishMessage(Serial.readString());
  mqttClient.loop();
}

void connectToNetwork(){
  if(WiFi.status() == WL_CONNECTED) return;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void connectToMqtt(){
    while (!mqttClient.connected()) {
      Serial.println("Connecting to MQTT...");
      if (mqttClient.connect("Node1-Module", mqttUser, mqttPassword )) {
        Serial.println("connected to MQTT");
      } else {
        Serial.println("failed with state: ");
        Serial.print(mqttClient.state());
        delay(2000);
      }
  }
}

void publishMessage(String JSONmessage){
  const size_t bufferSize = JSON_OBJECT_SIZE(6);
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& payload = jsonBuffer.parseObject(JSONmessage);
  const char* topic = payload["topic"]; 
  char dataToSend[150];
  char * data=(char *)malloc(sizeof(char)*150);
  getData((char*)JSONmessage.c_str(),data);
  if (mqttClient.publish(topic,data) == true) {
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
   free(data);
}


int strpos(char* string, char* substring)
{
   char * pointer = strstr(string, substring);
   if (pointer)
      return pointer - string;
   return -1; 
}
void getData(char *allpayload,char *data){
   int i=strpos(allpayload,"data")+strlen("data")+2;
   int j=0;
   while(allpayload[i]!='}'){
    data[j]=allpayload[i];
    j++;
    i++;
   }
   data[j]=allpayload[i];
   data[j+1]='\0';
}
void receiveMessage(char* topic, byte* payload, unsigned int length){
  StaticJsonBuffer<300> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  char JSONmessageBuffer[100];
  char* p = (char*)malloc(sizeof(char)*(length+1));
  strncpy(p,(char *)payload,length);
  p[length]='\0';
  JSONencoder["data"]=p;
  JSONencoder["topic"]=topic;
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.write(JSONmessageBuffer,strlen(JSONmessageBuffer));
  free(p);
}
