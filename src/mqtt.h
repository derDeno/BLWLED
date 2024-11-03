#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

const char *printerIp = "192.168.178.100"; // printer ip
String topic = String("device/") + "sn" + String("/report");

void mqtt_connect() {
    mqttClient.setServer(printerIp, 1883);
    mqttClient.setCallback(mqtt_listen);

    while (!mqttClient.connected()) {

        if (mqttClient.connect("BLWLED", "bblp", "accessCode")) {
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

void mqtt_listen(char* topic, byte* payload, unsigned int length) {}

void mqtt_parse() {}