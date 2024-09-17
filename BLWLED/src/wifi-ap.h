#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server, &dns);

void setup() {
  Serial.begin(115200);
  
  // Start the WiFi Manager in AP mode
  if (!WiFi.isConnected()) {
    wifiManager.autoConnect("ESP32-AP");
  }
  
  // Once connected to WiFi, you can proceed to other tasks
  if (WiFi.isConnected()) {
    Serial.println("Connected to WiFi!");
  }
}

void loop() {
  // Add your loop code
}
