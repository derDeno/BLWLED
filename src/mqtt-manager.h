#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClient wifiClient;
PubSubClient mqttClient;
String topic = String("device/") + appConfig.sn + String("/report");


void mqtt_listen(char* topic, byte* payload, unsigned int length) {
    Serial.println(topic);
}

void mqtt_parse() {}

int mqtt_connect() {
    if (strlen(appConfig.ip) == 0 || strlen(appConfig.ac) == 0 || strlen(appConfig.sn) == 0) {
        logger("Printer settings not configured");
        return 0;
    }

    mqttClient.setClient(wifiClient);
    mqttClient.setServer(appConfig.ip, 8883);
    mqttClient.setCallback(mqtt_listen);
    mqttClient.setSocketTimeout(15);

    int retries = 0;
    while (!mqttClient.connected() && retries < 5) {

        if (mqttClient.connect(appConfig.name, "bblp", appConfig.ac)) {
            logger("MQTT connected to printer");
            mqttClient.subscribe(topic.c_str());
            return 1;

        } else {
            logger("MQTT failed with state ");
            logger(String(mqttClient.state()));
            delay(2000);
            retries++;
        }
    }

    return 0;
}

void mqtt_disconnect() {}