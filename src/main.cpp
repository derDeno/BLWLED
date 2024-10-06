#include <FS.h>
#include <LITTLEFS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <Preferences.h>

#include <webserver.h>

String ssid = "Unbekannt";
String password = "ffYkexQAETVIb";
bool wifiSet;

Preferences pref;
DNSServer dnsServer;
AsyncWebServer server(80);

void setup() {
    Serial.begin(74880);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    logger("=============================");
    Serial.println("LittleFS mounted successfully");

    pref.begin("wifi", false);
    wifiSet = pref.getBool("setup");
    ssid = pref.getString("ssid", "");
    password = pref.getString("password", "");
    pref.end();

    // check if wifi was set or if in ap mode
    if (!wifiSet) {
      WiFi.softAP("BLWLED");
      dnsServer.start(53, "*", WiFi.softAPIP());
      logger("WiFi not setup yet, starting AP");

    }else {
      // Connect to Wi-Fi network
      WiFi.setHostname("BLWLED");
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      logger("Connecting to WiFi...");

      // Wait for connection
      while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print("... ");
      }

      logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
      logger(String("BLWLED Hostname: ") + String(WiFi.getHostname()));
      logger("RSSI: " + String(WiFi.RSSI()));
    }

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
}

void loop() {
    dnsServer.processNextRequest();
}