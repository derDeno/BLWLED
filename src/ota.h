#include <WiFi.h>
#include <ArduinoOTA.h>

void setup() {
  Serial.begin(115200);
  
  // Start WiFi (either AP or station mode)
  WiFi.begin("yourSSID", "yourPASSWORD");
  
  // Start OTA
  ArduinoOTA.begin();
  
  // Rest of your setup code...
}

void loop() {
  // Handle OTA update
  ArduinoOTA.handle();
  
  // Rest of your loop code...
}
