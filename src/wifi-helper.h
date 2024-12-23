#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <esp_task_wdt.h>

extern AppConfig appConfig;
extern AsyncWebServer server;
extern AsyncEventSource events;
extern Preferences pref;
extern PubSubClient mqttClient;
extern bool blockWifi;


static unsigned long lastAttemptTime = 0;
const unsigned long reconnectInterval = 100;

unsigned long lastScanTime = 0;
bool isScanning = false;


// setup mDNS
void setupMDNS() {
    const char* preferredHostname = "blwled";
    String hostname = String(appConfig.name);

    // Initialize mDNS with the fallback hostname
    if (!MDNS.begin(appConfig.name)) {
        logger("E: Failed to start mDNS with fallback hostname!");
        return;
    }
    delay(100);  // Allow mDNS to stabilize

    // Check if the preferred hostname is available
    if (static_cast<int>(MDNS.queryHost(preferredHostname)) == 0) {
        if (MDNS.begin(preferredHostname)) {
            hostname = preferredHostname;
        }
    }

    MDNS.addService("http", "tcp", 80);
    logger("Hostname set to http://" + hostname + ".local");
}

// setup WiFi
void setupWifi() {

    // Connect to Wi-Fi network
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    WiFi.setSleep(false);
    WiFi.setHostname(appConfig.name);
    WiFi.mode(WIFI_STA);
    WiFi.begin(appConfig.ssid, appConfig.pass);
    logger("Connecting to WiFi...");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        logger("... ");
    }

    // mDNS
    setupMDNS();

    // NTP
    configTime(0, 0, "pool.ntp.org");
    logger("NTP (UTC):    ok");
    logger("WiFI:         ok");
    logger("IP: " + WiFi.localIP().toString());
}

// change current connection for new connection
void changeWifi(String newSSID, String newPw) {

    // set the flag to prevent reconnecting
    blockWifi = true;

    const unsigned long startTime = millis();
    unsigned long timeout = 5000;

    // disconnect mqtt, events and wifi
    if (mqttClient.connected()) {
        mqttClient.disconnect();
        delay(500);
    }

    events.close();
    delay(500);
    WiFi.disconnect();
    delay(1000);

    Serial.printf("Free heap before connection: %d bytes\n", ESP.getFreeHeap());
    WiFi.begin(newSSID, newPw);

    unsigned long lastCheck = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < timeout)) {
        if (millis() - lastCheck > 500) { // Check every 500ms
            lastCheck = millis();
            Serial.println("Waiting for WiFi...");
        }
        esp_task_wdt_reset();
        delay(1);
    }

    Serial.print(WiFi.status());

    if (WiFi.status() == WL_CONNECTED) {
        logger("Successfully connected to the new WiFi network!");
        logger("New IP Address: " + WiFi.localIP().toString());

        WiFi.disconnect();
        delay(1000);
        setupWifi();

        blockWifi = false;

        events.onConnect([](AsyncEventSourceClient *client) {
            logger("SSE: Client reconnected");
            events.send("success", "network-switch", millis());
        });
        

        pref.begin("wifi");
        pref.putString("ssid", newSSID);
        pref.putString("pw", newPw);
        pref.end();

        delay(1000);
        ESP.restart();

    } else {
        logger("Failed to connect to the new WiFi network. Keeping current connection.");

        WiFi.disconnect();
        delay(500);

        setupWifi();

        events.onConnect([](AsyncEventSourceClient *client) {
            logger("SSE: Client reconnected");
            esp_task_wdt_reset();
            events.send("failed", "network-switch", millis());
            delay(1000);
            blockWifi = false;
        });
    }
}

// scan for networks
void scanNetworkLoop() {
    
    if (!isScanning) {
        return;
    }

    // Check if enough time has passed for the scan
    if (millis() - lastScanTime < 500) {
        return;
    }

    int scanResult = WiFi.scanComplete();
    if (scanResult == WIFI_SCAN_RUNNING) {
        lastScanTime = millis();
        return;
    }

    // Scan completed
    if (scanResult >= 0) {

        // Send each network found as an event
        for (int i = 0; i < scanResult; ++i) {

            String jsonMessage;
            JsonDocument doc;
            doc["ssid"] = WiFi.SSID(i);
            doc["rssi"] = WiFi.RSSI(i);
            doc["bssid"] = WiFi.BSSIDstr(i);
            doc["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";
            serializeJson(doc, jsonMessage);

            events.send(jsonMessage.c_str(), "network", millis());
            delay(10);
        }

        WiFi.scanDelete();
        Serial.println("WiFi scan complete");

    } else if (scanResult == WIFI_SCAN_FAILED) {
        Serial.println("WiFi scan failed");
    }

    // Reset scanning state
    isScanning = false;
}

// check scanning state for the api
bool startNetworkScan() {
    if (isScanning) {
        return false;
    }

    WiFi.scanNetworks(true);
    isScanning = true;
    lastScanTime = millis();

    return true;
}



// setup in AP Mode if no WiFi set
void setupWifiAp() {
    WiFi.softAP("BLWLED");
}

// loop for WiFi
void wifiLoop() {

    // check wifi connection
    if (WiFi.status() != WL_CONNECTED) {
        if (millis() - lastAttemptTime > reconnectInterval) {
            lastAttemptTime = millis();
            logger("W: Lost WiFi connection. Attempting reconnect...");
            WiFi.disconnect();
            WiFi.reconnect();
        }
    }


    // wifi scan loop
    scanNetworkLoop();
}