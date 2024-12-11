#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClientSecure wifiClient;
PubSubClient mqttClient;
String topic = String("device/") + appConfig.sn + String("/report");


void mqtt_listen(char* topic, byte* payload, unsigned int length) {
    logger(topic);
}


void mqtt_parse() {}


int mqtt_reconnect() {
    // check if wifi is connected
    if (WiFi.status() != WL_CONNECTED) {
        return 0;
    }

    // if mqtt connected, disconnect first
    if(mqttClient.connected()) {
        mqttClient.disconnect();
    }

    int retries = 0;
    while (!mqttClient.connected() && retries < 5) {

        if (mqttClient.connect(appConfig.name, "bblp", appConfig.ac)) {
            logger("MQTT connected to printer");
            mqttClient.subscribe(topic.c_str());
            return 1;

        } else {
            logger("E:  MQTT failed with state " + String(mqttClient.state()));
            delay(2000);
            retries++;
        }
    }

    logger("E:  Failed to connect to MQTT after 5 retries");
    return 0;
}


bool mqtt_setup() {

    if (!appConfig.printerSet) {
        logger("E:  Printer not configured!");
        return false;
    }

    wifiClient.setInsecure();

    mqttClient.setClient(wifiClient);
    mqttClient.setBufferSize(1024);
    mqttClient.setServer(appConfig.ip, 8883);
    mqttClient.setCallback(mqtt_listen);
    mqttClient.setSocketTimeout(20);

    return true;
}