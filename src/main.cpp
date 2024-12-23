#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <time.h>

#include "config.h"
#include "log.h"
#include "wifi-manager.h"
#include "led-manager.h"
#include "mqtt-manager.h"
#include "action.h"
#include "events.h"
#include "webserver.h"


AppConfig appConfig;
AsyncWebServer server(80);
AsyncEventSource events("/events");
Preferences pref;
CRGB* leds;

int swState = HIGH;
int lastSwState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned const long debounceDelay = 1000;
unsigned int wifireconnect = 0;

void initState() {
    pref.begin("deviceSettings");

    // set the board name (aka hostname) using 3 mac bytes
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char boardName[14];
    snprintf(boardName, sizeof(boardName), "BLWLED-%02X%02X%02X", mac[3], mac[4], mac[5]);

    if (pref.getString("name", "").length() == 0) {
        pref.putString("name", boardName);
    }

    strcpy(appConfig.name, pref.getString("name", boardName).c_str());
    appConfig.wled = pref.getBool("wled", PREF_WLED);
    appConfig.count = pref.getInt("count", PREF_COUNT);
    strcpy(appConfig.order, pref.getString("order", PREF_ORDER).c_str());
    appConfig.analog = pref.getBool("analog", PREF_ANALOG);
    appConfig.mode = pref.getInt("mode", PREF_MODE);
    appConfig.sw = pref.getBool("sw", PREF_SW);
    appConfig.action = pref.getInt("action", PREF_ACTION);
    appConfig.logging = pref.getBool("logging", PREF_LOGGING);
    pref.end();

    pref.begin("printerSettings", true);
    strcpy(appConfig.ip, pref.getString("ip", PREF_IP).c_str());
    strcpy(appConfig.ac, pref.getString("ac", PREF_AC).c_str());
    strcpy(appConfig.sn, pref.getString("sn", PREF_SN).c_str());
    appConfig.rtid = pref.getBool("rtid", PREF_RTID);
    appConfig.rtit = pref.getInt("rtit", PREF_RTIT);
    pref.end();

    pref.begin("wifi", true);
    appConfig.wifiSet = pref.getBool("set", true);
    strcpy(appConfig.ssid, pref.getString("ssid", PREF_SSID).c_str());
    strcpy(appConfig.pass, pref.getString("pass", PREF_PASS).c_str());
    pref.end();

    appConfig.printerSet = (strlen(appConfig.ip) > 0 && strlen(appConfig.ac) > 0 && strlen(appConfig.sn) > 0) ? true : false;
}

void setup() {
    Serial.begin(74880);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    logger("=============================");
    logger("LittleFS mounted successfully");

    // Initialize application state
    initState();
    setupWled();

    if (!appConfig.wifiSet) {
        logger("WiFi not setup yet, starting AP");
        setupWifiAp();
    } else {
        setupWifi();
    }

    // pins def
    pinMode(SW_PIN, INPUT_PULLUP);
    pinMode(ANALOG_PIN_R, OUTPUT);
    pinMode(ANALOG_PIN_G, OUTPUT);
    pinMode(ANALOG_PIN_B, OUTPUT);
    pinMode(ANALOG_PIN_CW, OUTPUT);
    pinMode(ANALOG_PIN_WW, OUTPUT);
    pinMode(WLED_PIN, OUTPUT);

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
    logger(String(appConfig.name) + " is ready!");

    startupAnimation();

    // start mqtt
    if (mqtt_setup()) {
        mqtt_reconnect();
    }
}

void loop() {
    // react to switch press
    int reading = digitalRead(5);
    if (reading != lastSwState) {
        lastDebounceTime = millis();

        if (reading == LOW) {
            eventBus(EVENT_SW_CLICK);
        }
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
    }
    lastSwState = reading;

    // capture wifi disconnect
    if (WiFi.status() != WL_CONNECTED) {
        logger("W:  Lost WiFi connection");
        logger("Retrying to connect to " + String(appConfig.ssid));

        wifireconnect += 1;

        if (wifireconnect < 10) {
            WiFi.disconnect();
            delay(100);
            WiFi.reconnect();
        } else {
            // 10 reconnection attempts failed, try other methods
        }
    }

    // capture mqtt messages
    mqttClient.loop();
}
