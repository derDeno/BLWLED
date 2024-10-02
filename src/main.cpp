#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LITTLEFS.h>
#include <WiFi.h>

#include <log.h>
#include <webserver.h>

const char *ssid = "Unbekannt";
const char *password = "ffYkexQAETVIb";

AsyncWebServer server(80);

void setup() {
    // Initialize Serial Monitor
    Serial.begin(74880);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    Serial.println("LittleFS mounted successfully");

    // Connect to Wi-Fi network
    WiFi.setHostname("BLWLED-4");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    logger("Connecting to WiFi...");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print("... ");
    }

    logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
    logger(String("BLWLED HostName: ") + String(WiFi.getHostname()));
    logger("RSSI: " + String(WiFi.RSSI()));

    

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
}

void loop() {
    // Nothing needed here for now
}