#include <Arduino.h>
#include <config.h>
#include <connections.h>

config cfg;

void setup() {
  Serial.begin(115200);
  cfg.load();
  irsend.begin();

  setupWiFi(cfg);
  setupOTA(cfg);
  setupMQTT(cfg);
}

void loop() {
  MDNS.update();
  ArduinoOTA.handle();
  yield(); // Stop ESP8266 from crashing, yea must yield.
  digitalWrite(LED_BUILTIN, HIGH);
  if (!MQTTclient.connected())
  {
    connectMQTT(cfg);
  }
  MQTTclient.loop();
}