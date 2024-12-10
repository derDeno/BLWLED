#include <Arduino.h>
#include <ESPmDNS.h>

extern AppConfig appConfig;
extern Preferences pref;

//setup mDNS
void setupMDNS() {
    if (MDNS.begin("blwled")) {
        logger("Hostname set to http://blwled.local");

    } else {
        // Preferred hostname failed, try the fallback
        String hostname = String(appConfig.name);
        Serial.println("Preferred hostname 'blwled' is already in use, trying fallback...");
        if (MDNS.begin(hostname)) {
            logger("Hostname set to http://" + hostname +".local");
            
        } else {
            logger("Failed to start mDNS with fallback hostname!");
        }
    }
}

// setup WiFi
void setupWifi() {
    const char* ssid = "Unbekannt";
    const char* password = "ffYkexQAETVIb";
    
    // Connect to Wi-Fi network
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    WiFi.setSleep(false);
    WiFi.setHostname(appConfig.name);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
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
    logger("NTP time is synchronized. Time is in UTC");

    logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
    logger(String("BLWLED Hostname: ") + String(WiFi.getHostname()));
    logger("RSSI: " + String(WiFi.RSSI()));
}

// change current connection for new connection
int changeWifi(String newSSID, String newPw) {
    
    WiFiMulti wifiMulti;
    wifiMulti.addAP(newSSID.c_str(), newPw.c_str());
    const unsigned long startTime = millis();
    unsigned long timeout = 10000;

    // test new connection
    logger("Testing new WiFi connection");
    while (wifiMulti.run() != WL_CONNECTED && millis() - startTime < timeout) {
        delay(500);
        Serial.print(".");
    }

    if (wifiMulti.run() == WL_CONNECTED) {
        logger("\nSuccessfully connected to the new WiFi network!");
        logger("New IP Address: " + WiFi.localIP().toString());

        pref.begin("wifi");
        pref.putString("ssid", newSSID);
        pref.putString("pw", newPw);
        pref.end();
    
        return 1;
    } else {
        return 0;
    }
}

// scan for Networs
void scanNetworks() {

}

// setup in AP Mode if no WiFi set
void setupWifiAp() {
    WiFi.softAP("BLWLED");
}