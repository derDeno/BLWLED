#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);


const char *clientId = "BLWLED-"; // generate unique number using mac address
const char *topic = "emqx/esp32";
const int mqtt_port = 1883;



void mqtt_connect() {
    mqttClient.setServer(mqtt_broker, mqtt_port);
    mqttClient.setCallback(callback);

    while (!mqttClient.connected()) {

        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());

        if (mqttClient.connect(client_id.c_str(), "blp", "sn")) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(mqttClient.state());
            delay(2000);
        }
    }
}

void mqtt_disconnect() {}

void mqtt_listen() {}

void mqtt_parse() {}