#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClientSecure wifiClient;
PubSubClient mqttClient;
String topic = String("device/") + appConfig.sn + String("/report");


void mqtt_listen(char* topic, byte* payload, unsigned int length) {
    Serial.println(topic);
}


void mqtt_parse() {}


int mqtt_connect() {
    if (WiFi.status() != WL_CONNECTED) {
        return 0;
    }

    int retries = 0;
    while (!mqttClient.connected() && retries < 5) {

        if (mqttClient.connect(appConfig.name, "bblp", appConfig.ac)) {
            logger("MQTT connected to printer");
            mqttClient.subscribe(topic.c_str());
            return 1;

        } else {
            logger("MQTT failed with state " + String(mqttClient.state()));
            delay(2000);
            retries++;
        }
    }

    logger("Failed to connect to MQTT after 5 retries");
    return 0;
}


void mqtt_setup() {
    // if connected, disconnect first
    if(mqttClient.connected()) {
        mqttClient.disconnect();
    }

    if (strlen(appConfig.ip) == 0 || strlen(appConfig.ac) == 0 || strlen(appConfig.sn) == 0) {
        logger("Printer settings not configured");
    }

    wifiClient.setInsecure();

    mqttClient.setClient(wifiClient);
    mqttClient.setBufferSize(1024);
    mqttClient.setServer(appConfig.ip, 8883);
    mqttClient.setCallback(mqtt_listen);
    mqttClient.setSocketTimeout(20);
}