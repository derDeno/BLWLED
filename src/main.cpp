#include <FS.h>
#include <LITTLEFS.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

const char* ssid = "Unbekannt";
const char* password = "ffYkexQAETVIb";

AsyncWebServer server(80);

void setup(){
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  Serial.println("LittleFS mounted successfully");

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.hostname("BLWLED-4");
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Serve static files from LittleFS
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  // Nothing needed here for now
}