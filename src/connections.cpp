#include <connections.h>

WiFiClient wclient;
PubSubClient MQTTclient(wclient);
IRsend irsend(kIrLed); 

char MQTT_PWR[64];      //On/Off - 0xD81
char MQTT_FAN[64];      //Fan Speed - 0xD90
char MQTT_OSC[64];      //Oscillate - 0xDA0
char MQTT_TIM[64];      //Timer - 0xDDA 
char MQTT_WIN[64];      //Wind Type - 0xDC3

void setupWiFi(config cfg){
    Serial.println("WiFi setup");
    WiFi.mode(WIFI_STA);
    WiFi.hostname(cfg.device_id);
    WiFi.begin(cfg.wifi.ssid, cfg.wifi.password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.println("WiFi ready!");
    Serial.print("Connected to ");
    Serial.println(cfg.wifi.ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setupOTA(config cfg){
    ArduinoOTA.setHostname(cfg.device_id);

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_FS
            type = "filesystem";
            SPIFFS.end();
        }
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
    ArduinoOTA.begin(true);
}

void callbackMQTT(char* p_topic, byte* p_payload, unsigned int p_length) {
    // concat the payload into a string
    String payload;
    for (uint8_t i = 0; i < p_length; i++) {
        payload.concat((char)p_payload[i]);
    }
    //Debug payload data to serial console
    Serial.println(p_topic);
    Serial.println(payload);

    if (strcmp(MQTT_PWR,p_topic) == 0) {
        if(p_length>0){ //check we have any payloads
            irsend.sendSymphony(0xD81, 12, 2);
        }
    }
    if (strcmp(MQTT_FAN,p_topic) == 0) {
        if(p_length>0){ //check we have any payloads
            irsend.sendSymphony(0xD90, 12, 2);
        }
    }  
    if (strcmp(MQTT_OSC,p_topic) == 0) {
        if(p_length>0){ //check we have any payloads
            irsend.sendSymphony(0xDA0, 12, 2);
        }
    }  
    if (strcmp(MQTT_TIM,p_topic) == 0) {
        if(p_length>0){ //check we have any payloads
            irsend.sendSymphony(0xDDA, 12, 2);
        }
    }  
    if (strcmp(MQTT_WIN,p_topic) == 0) {
        if(p_length>0){ //check we have any payloads
            irsend.sendSymphony(0xDC3, 12, 2);
        }
    }    
}

void setupMQTT(config cfg){
    MQTTclient.setServer(cfg.mqtt.host, cfg.mqtt.port);
    MQTTclient.setCallback(callbackMQTT);
}

void connectMQTT(config cfg) {
    strcpy(MQTT_PWR, cfg.mqtt.base_topic);
    strcat(MQTT_PWR, cfg.device_id);
    strcat(MQTT_PWR, "/pwr"); 
    strcpy(MQTT_FAN, cfg.mqtt.base_topic);
    strcat(MQTT_FAN, cfg.device_id);
    strcat(MQTT_FAN, "/fan");
    strcpy(MQTT_OSC, cfg.mqtt.base_topic);
    strcat(MQTT_OSC, cfg.device_id);
    strcat(MQTT_OSC, "/osc");  
    strcpy(MQTT_TIM, cfg.mqtt.base_topic);
    strcat(MQTT_TIM, cfg.device_id);
    strcat(MQTT_TIM, "/tim");     
    strcpy(MQTT_WIN, cfg.mqtt.base_topic);
    strcat(MQTT_WIN, cfg.device_id);
    strcat(MQTT_WIN, "/win");    
    // Loop until we're reconnected
    while (!MQTTclient.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (MQTTclient.connect(cfg.device_id, cfg.mqtt.username, cfg.mqtt.password)) {
            char topic[64];
            strcpy(topic, cfg.mqtt.base_topic);
            strcat(topic, cfg.device_id);
            strcat(topic, "/#");
            MQTTclient.subscribe(topic);
            Serial.println("connected");
            // Once connected, return
        } else {
            Serial.print("failed, rc=");
            Serial.print(MQTTclient.state());
            Serial.println(" try again in 5 seconds");
            delay(5e3);
        }
    }
}
