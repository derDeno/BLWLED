#include <FS.h>
#include <LITTLEFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <time.h>

const char *ssid = "Unbekannt";
const char *password = "ffYkexQAETVIb";

AsyncWebServer server(80);
String processorInfo(const String &var);

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}


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
  Serial.println("Connecting to WiFi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("... ");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP32 HostName: ");
  Serial.println(WiFi.getHostname());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

  // Serve static files from LittleFS
  server.serveStatic("/", LittleFS, "/").setFilter(ON_STA_FILTER);
  server.serveStatic("/", LittleFS, "/captive.html").setFilter(ON_AP_FILTER);

  // routing
  server.serveStatic("/info", LittleFS, "/info.html").setTemplateProcessor(processorInfo);
/*
  server.on("/info", HTTP_GET, [] (AsyncWebServerRequest *request) {
        request->send(LittleFS, "/info.html", String(), false, processorInfo);
    });
*/

  server.onNotFound(notFound);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Nothing needed here for now
}


String processorInfo(const String &var) {
  if (var == "TEMPLATE_MAC") {
    return WiFi.macAddress();

  } else if(var == "TEMPLATE_IP") {
    return WiFi.localIP().toString();

  } else if(var == "TEMPLATE_HOSTNAME") {
    return F(WiFi.getHostname());

  } else if(var == "TEMPLATE_RSSI") {
    return F(WiFi.RSSI());

  } else if(var == "TEMPLATE_VERSION") {
    return F("0.0.1-b");

  } else if(var == "TEMPLATE_UPTIME") {
    unsigned long uptimeMillis = millis();

    unsigned long seconds = uptimeMillis / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long days = hours / 24;

    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;

    String uptime = String(days) + " days, " + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);
    return uptime;

  } else if(var == "TEMPLATE_LOCAL_TIME") {
    return F("to be added");

  }
  return String();
}

