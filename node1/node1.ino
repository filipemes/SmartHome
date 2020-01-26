#include "RunningMedian.h"
#include <ArduinoJson.h>
#include <SoftwareSerial.h> 
#include <dht.h>
#include <SimpleKalmanFilter.h>

//Global Variables
RunningMedian gasSamples = RunningMedian(20);
SoftwareSerial serialAux(2,3); //rx tx
SimpleKalmanFilter temperatureKalman = SimpleKalmanFilter(2, 2, 0.01);
SimpleKalmanFilter humidityKalman = SimpleKalmanFilter(5, 5, 0.01);
const int gasSensorPin = A0;
const int temperatureSensorPin = 5;
const int fanPin=4;
int times=0;
int temperatureThreshold=23;
float temperature;
float humidity;
boolean isFanOn=false;
dht DHT;

//Declared Functions
void sendStateNode1();
void gasDetection();
void stringProcessing(String inputString);
void sendAlert();
void temperatureReading();
void humidityReading();
void turnOnFan();
void turnOffFan();

//Setup Function
void setup(){
  pinMode(gasSensorPin, INPUT); 
  pinMode(fanPin, OUTPUT); 
  Serial.begin(9600);
  serialAux.begin(9600);
  digitalWrite(fanPin,HIGH);
}

void loop(){
  if(serialAux.available())
    stringProcessing(serialAux.readString());
  int chk = DHT.read11(temperatureSensorPin);;
  temperatureReading();
  humidityReading();
  delay(200);
  gasDetection();
  times++;
  if(times==5){
    if(gasSamples.getMedian()>400){
        sendAlert();
        gasSamples = RunningMedian(200);
        delay(1000);
    }
    sendStateNode1();
    times=0;
  }
  delay(1000);
}

void stringProcessing(String message){
  StaticJsonBuffer<200> jsonBuffer;
  StaticJsonBuffer<100> jsonBufferData;
  JsonObject& payload = jsonBuffer.parseObject(message);
  JsonObject& payloadData = jsonBufferData.parseObject((char *)payload["data"]);
  Serial.println((char *)payload["topic"]);
  if(strcmp(payload["topic"],"setFanState")==0&&strcmp((char *)payloadData["fanState"],"true")==0)
     turnOnFan();
     //Turn On Air Conditioning
  else if(strcmp(payload["topic"],"setFanState")==0&&strcmp((char *)payloadData["fanState"],"false")==0)
     turnOffFan();
     //Turn Off Air Conditioning
  else if(strcmp(payload["topic"],"setThreshold")==0)
      temperatureThreshold=atoi((char *)payloadData["threshold"]); 
  
}

void gasDetection(){
  int analogGasSensorValue = analogRead(gasSensorPin);
  gasSamples.add(analogGasSensorValue);
}

void temperatureReading(){
  temperature=temperatureKalman.updateEstimate(DHT.temperature);;
  if(ceil(temperature)>temperatureThreshold)
     turnOnFan();
  else
     turnOffFan();
}
void humidityReading(){
  humidity=humidityKalman.updateEstimate(DHT.humidity);
}

void sendStateNode1(){
  StaticJsonBuffer<200> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["topic"]="updateStateNode1";
  JsonObject& data = JSONencoder.createNestedObject("data");
  data["fanState"]= isFanOn ? "true" : "false";
  data["gasConcentration"]=ceil(gasSamples.getMedian());
  data["temperature"] =ceil(temperature);
  data["humidityPercentage"]=ceil(humidity);
  data["threshold"]=temperatureThreshold;
  char JSONmessageBuffer[200];
  JSONencoder.printTo(JSONmessageBuffer,sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
  serialAux.write(JSONmessageBuffer,strlen(JSONmessageBuffer));
}
void sendAlert(){
  StaticJsonBuffer<100> JSONbuffer;
  JsonObject& JSONencoder = JSONbuffer.createObject();
  JSONencoder["topic"]="notifyGasAlert";
  JsonObject& data = JSONencoder.createNestedObject("data");
  data["gasConcentration"]=ceil(gasSamples.getMedian());
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer,sizeof(JSONmessageBuffer));
  serialAux.write(JSONmessageBuffer,strlen(JSONmessageBuffer));
}
void turnOnFan(){
  isFanOn=true;
  digitalWrite(fanPin,LOW);
}
void turnOffFan(){
  isFanOn=false;
  digitalWrite(fanPin,HIGH);
}
