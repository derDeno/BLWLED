#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure wifiSecureClient;
PubSubClient mqttClient(wifiSecureClient);

String device_topic;
String report_topic;
String clientId = "BLWLED-";