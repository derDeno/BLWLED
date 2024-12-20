#include <Arduino.h>
#include <ESPmDNS.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

extern AppConfig appConfig;
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

    MDNS.addService("http", "tcp", 80);
    delay(100);  // Allow mDNS to stabilize

    // Check if the preferred hostname is available
    if (static_cast<int>(MDNS.queryHost(preferredHostname)) == 0) {
        if (MDNS.begin(preferredHostname)) {
            MDNS.addService("http", "tcp", 80);
            hostname = preferredHostname;
            return;
        }
    }

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
    bool newNetworkSuccess = false;

    WiFiMulti wifiMulti;
    wifiMulti.addAP(newSSID.c_str(), newPw.c_str());
    const unsigned long startTime = millis();
    unsigned long timeout = 10000;

    // test new connection
    if (mqttClient.connected()) {
        mqttClient.disconnect();
        delay(500);
    }
    WiFi.disconnect();
    delay(1000);
    WiFi.mode(WIFI_STA);

    Serial.printf("Free heap before connection: %d bytes\n", ESP.getFreeHeap());

    while (millis() - startTime < timeout) {
        Serial.println("DEBUG 1");
        int wifiStatus = wifiMulti.run();
        Serial.println("DEBUG 2");

        if (wifiStatus == WL_CONNECTED) {
            Serial.println("DEBUG 5");
            newNetworkSuccess = true;
            break;
        } else if (wifiStatus == WL_CONNECT_FAILED) {
            Serial.println("DEBUG 6");
            Serial.println("Connection failed: Incorrect password or SSID.");
            break;
        } else if (wifiStatus == WL_NO_SHIELD) {
            Serial.println("DEBUG 4");
            Serial.println("WiFi shield not initialized.");
            break;
        } else {
            Serial.println("DEBUG 7");
        }

        delay(500);
        yield(); 
    }

    if (newNetworkSuccess) {
        logger("Successfully connected to the new WiFi network!");
        logger("New IP Address: " + WiFi.localIP().toString());

        WiFi.disconnect();
        delay(500);
        setupWifi();

        blockWifi = false;
        events.send("success", "network-switch", millis());

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

        blockWifi = false;
        events.send("failed", "network-switch", millis());
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
            String ssid = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);
            String encryptionType = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";

            String jsonMessage;
            JsonDocument doc;
            doc["ssid"] = ssid;
            doc["rssi"] = rssi;
            doc["encryption"] = encryptionType;
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