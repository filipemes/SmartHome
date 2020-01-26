#!/usr/bin/env python3

import paho.mqtt.client as mqtt
from pyfcm import FCMNotification
from time import gmtime, strftime
import pyrebase
import json
import _thread

############# Setup configs ########
config = {
    "apiKey": "AIzaSyDEG9Nvskj3i6PiQ02rw206Rw8l0Ip_Vks",
    "authDomain": "577330970066.firebaseapp.com",
    "databaseURL": "https://smarthomecloud-3bfc4.firebaseio.com",
    "storageBucket": "smarthomecloud-3bfc4.appspot.com"
}
firebase = pyrebase.initialize_app(config)
realTimeDatabase = firebase.database()
push_service = FCMNotification(api_key="AAAAhmubfdI:APA91bFB7IVfzvwjQP1qM-IMdMD1fDdwC5HstLYDbnbSy3FQPtYzFnpgixxtWGqbflYSWRGp28TTHstWdn4m4he1zi514zlm5VTCFIh4fSrXP_I3g-nc9xWpyofsNBGcrd6jFkEtoV4P")

############# Functions ########
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("**** Connected to broker ****")
    client.subscribe("updateStateNode1")
    client.subscribe("updateMoisturePercentage")
    client.subscribe("updateWatering")
    client.subscribe("notifyGasAlert")

def on_message(client, userdata, msg):
    if len(str(msg.payload.decode("utf-8","ignore")))>0:
        payload=json.loads(str(msg.payload.decode("utf-8","ignore")))
        
    time=strftime("%d-%m-%Y %H:%M", gmtime())
    if msg.topic=='updateMoisturePercentage':
        print("Log: Received a message with topic updateMoisturePercentage")
        realTimeDatabase.child("node2").update({"lastMeasure":time,"moisturePercentage":payload["moisturePercentage"]})
    elif msg.topic=='updateStateNode1':
        print("Log: Received a message with topic updateStateNode1")
        print(str(msg.payload.decode("utf-8","ignore")))
        realTimeDatabase.child("node1").update({"fanState":payload["fanState"],"threshold":payload["threshold"],"temperature":payload["temperature"],"humidityPercentage":payload["humidityPercentage"],"gasConcentration":payload["gasConcentration"]})
    elif msg.topic=='updateWatering':
        print("Log: Received a message with topic updateWatering")
        realTimeDatabase.child("node2").update({"lastWatering":time})
    elif msg.topic=='notifyGasAlert':
        print("Log: Received a message with topic notifyGasAlert")
        realTimeDatabase.child("node1").update({"lastAlert":time,"gasConcentration":payload["gasConcentration"]})
        registration_id = "euDVbMZD9vA:APA91bEuTJ1hsQvNASkB2c314PMf3BwH2-4OuXuQFn2-l9YdyqolplbPCKtMdSj_dHc6Jo2vNBoKoauOauZFmcKTTXjceYF31jcAl4s_QdwY0oVYiaZubZs5ViF0ghXRLIyuhy8y9IRv"
        message_title = "Alert - Gas Notification"
        message_body = "Unusual Gas Values, call the authorities"
        result = push_service.notify_single_device(registration_id=registration_id, message_title=message_title, message_body=message_body)
        print(result)

def stream_handler(message):
    if message["path"]=="/fanState":
        payloadToSend={
            "fanState":message["data"]
        }
        mqttClient.publish("setFanState",json.dumps(payloadToSend))
        print("Published a message with topic setFanState")
    elif message["path"]=="/threshold":
        payloadToSend={
            "threshold":message["data"]
        }
        mqttClient.publish("setThreshold",json.dumps(payloadToSend))
        print("Published a message with topic setThreshold")

def starting_firebase_worker_thread():
    realTimeDatabase.child("node1").stream(stream_handler)

mqttClient = mqtt.Client("Gateway")
mqttClient.username_pw_set("smartHomeGateway", password="1234Isep")
mqttClient.on_connect = on_connect
mqttClient.on_message = on_message
mqttClient.connect("192.168.43.127",12948,60)
try:
    try:
        _thread.start_new_thread( starting_firebase_worker_thread,())
    except:
        print ("Error: unable to start thread")
    mqttClient.loop_forever()
except KeyboardInterrupt:
    print("Exiting...")
    mqttClient.disconnect()
    mqttClient.loop_stop()
