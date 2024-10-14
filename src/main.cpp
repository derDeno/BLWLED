#include "esp_sntp.h"
#include "webserver.h"

String ssid = "Unbekannt";
String password = "ffYkexQAETVIb";
bool wifiSet = true;

AsyncWebServer server(80);

void initWifi() {
  // Connect to Wi-Fi network
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.setSleep(false);
  WiFi.setHostname("BLWLED");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  logger("Connecting to WiFi...");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    logger("... ", false);
  }

  // NTP
  esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
  esp_netif_sntp_init(&config);

  logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
  logger(String("BLWLED Hostname: ") + String(WiFi.getHostname()));
  logger("RSSI: " + String(WiFi.RSSI()));
}

void setup() {
  Serial.begin(74880);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  logger("=============================");
  logger("LittleFS mounted successfully", false);

  // check prefs if wifi is already setup, else start ap mode

  /* for dev, deactivate
  pref.begin("wifi", false);
  wifiSet = pref.getBool("setup", false);
  ssid = pref.getString("ssid", "");
  password = pref.getString("pw", "");
  pref.end();
  */

  if (!wifiSet) {
    WiFi.softAP("BLWLED");
    logger("WiFi not setup yet, starting AP");

  } else {
    initWifi();
  }

  // Start server
  routing(server);
  server.begin();
  logger("HTTP server started");

  // Onboard Switch def
  pinMode(5, INPUT);
}

void loop() {

  // react to switch press
  int swState = digitalRead(5);
  if (swState == LOW) {
    logger("Onboard switch pressed");
    pref.begin("deviceSettings", true);
    bool swActive = pref.getBool("sw");
    String action = pref.getString("function", "");
    pref.end();

    if(swActive) {
      if(action == "event") {
        // TODO: check for mapping
      }else if(action == "reboot") {
        ESP.restart();
      }else {
        // do nothing
      }
    }
    delay(1000); // debounce
  }

}