#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include "RunningMedian.h"
#include <ESP8266HTTPClient.h>

const int moistureSensorPin = A0;
const int waterPumpPin = 14;

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
void publishMessage(char* JSONmessageBuffer,char * topic);
void receiveMessage(char* topic, byte* payload, unsigned int length);
void connectToNetwork();
float getMoisturePercentage();
boolean checkRainForecast(char* forecast);
char * getForecast();
int strpos(char* string,char* substring);

void setup() {
  pinMode(moistureSensorPin,INPUT);
  pinMode(waterPumpPin,OUTPUT);
  digitalWrite(waterPumpPin,HIGH);
  Serial.begin(115200);
  connectToNetwork();
  connectToMqtt();
  char * forecast=getForecast();
  float moisturePercentage=getMoisturePercentage();
  if(checkRainForecast(forecast)==false&&(int)moisturePercentage<70){
    digitalWrite(waterPumpPin,LOW); //turn On WaterPump
    delay(3000);//water the plants during 3sec
    digitalWrite(waterPumpPin,HIGH); //turn Off Water Pump;
    publishMessage("","updateWatering");
  }else{
    StaticJsonBuffer<300> JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject();
    JSONencoder["moisturePercentage"] = (int)moisturePercentage;
    char JSONmessageBuffer[100];
    JSONencoder.printTo(JSONmessageBuffer,sizeof(JSONmessageBuffer));
    publishMessage(JSONmessageBuffer,"updateMoisturePercentage");
  }
  free(forecast);
  //ESP.deepSleep(30e6);//30sec
  ESP.deepSleep(4.32e10);//12h
}

void loop() {}

boolean checkRainForecast(char *forecast){
  int i=strpos(forecast,"precipProbability")+strlen("precipProbability")+2;
  char precipProbability[4];
  int j=0;
  while(forecast[i]!=','){
    precipProbability[j]=forecast[i];
    i++;
    j++;
  }
  precipProbability[j]='\0';
  if((atof(precipProbability)*100)>80)
    return true; 
  else
    return false;
}

int strpos(char* string, char* substring)
{
   char * pointer = strstr(string, substring);
   if (pointer)
      return pointer - string;
   return -1; 
}

char* getForecast(){
  HTTPClient http;
  char * string=NULL;
  http.begin("https://api.darksky.net/forecast/d68d3893a5de8a09cd2bb573d916c848/41.1579,-8.6291?exclude=hourly,alerts,flags,minutely,currently");  
  http.addHeader("Accept","application/json;charset=UTF-8");
  int httpCode = http.GET();                                                 
  if (httpCode == 200) { 
    int len = http.getSize();
    char char_buff[1];
    WiFiClient * stream = http.getStreamPtr();
    string=(char *)malloc(len+1);
    int i=0;
    while (http.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(char_buff, ((size > sizeof(char_buff)) ? sizeof(char_buff) : size));
        string[i]=char_buff[0];
        if (len > 0)
          len -= c;
        i++;
      }
    }
    string[i]='\0';
  }
  http.end(); 
  return string;
}

float getMoisturePercentage(){
  RunningMedian samplesMoisture = RunningMedian(50);
  for(int i=0;i<50;i++){
      samplesMoisture.add(analogRead(moistureSensorPin));
      delay(1000);
  }
  return map(samplesMoisture.getMedian(),1023,358,0,100);
}

void connectToNetwork(){
  WiFi.begin(ssid, password);
  if(WiFi.status() == WL_CONNECTED) return;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
}

void connectToMqtt(){
    mqttClient.setServer(mqttServer, mqttPort);
    while (!mqttClient.connected()) {
      Serial.println("Connecting to MQTT...");
      if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword )) {
        Serial.println("connected to MQTT");
      } else {
        Serial.println("failed with state: ");
        Serial.print(mqttClient.state());
        delay(2000);
      }
  }
}


void publishMessage(char * JSONmessage,char * topic){
  if (mqttClient.publish(topic, JSONmessage) == true) {
    Serial.println(JSONmessage);
    Serial.println("Success sending message");
  } else {
    Serial.println("Error sending message");
  }
}
