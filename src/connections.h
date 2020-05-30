#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <config.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t kIrLed = D1; 

extern WiFiClient wclient;
extern PubSubClient MQTTclient;
extern IRsend irsend;
  
void setupWiFi(config cfg);
void setupOTA(config cfg);
void setupMQTT(config cfg);
void connectMQTT(config cfg);
#endif 