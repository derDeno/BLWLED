#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);
String topic = String("device/") + appConfig.sn + String("/report");


void mqtt_listen(char* topic, byte* payload, unsigned int length) {
    Serial.println(topic);
}

void mqtt_parse() {}

void mqtt_connect() {
    if (strlen(appConfig.ip) == 0 || strlen(appConfig.ac) == 0 || strlen(appConfig.sn) == 0) {
        logger("Printer settings not configured");
        return;
    }

    mqttClient.setServer(appConfig.ip, 1883);
    mqttClient.setCallback(mqtt_listen);

    while (!mqttClient.connected()) {

        if (mqttClient.connect(appConfig.name, "bblp", appConfig.ac)) {
            Serial.println("MQTT connected");
            mqttClient.subscribe(topic.c_str());
        } else {
            Serial.print("failed with state ");
            Serial.print(mqttClient.state());
            delay(2000);
        }
    }
}

void mqtt_disconnect() {}