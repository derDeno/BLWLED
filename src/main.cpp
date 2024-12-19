#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <time.h>

#include "fs-helper.h"
#include "config.h"
#include "log.h"
#include "wifi-helper.h"
#include "led-helper.h"
#include "action.h"
#include "events.h"
#include "mqtt-helper.h"
#include "webserver.h"


AppConfig appConfig;
AsyncWebServer server(80);
AsyncEventSource events("/api/events");
Preferences pref;
CRGB* leds;

int swState = HIGH;
int lastSwState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned const long debounceDelay = 1000;


void initState() {
    
    // FS Version
    char versionBuffer[13];
    readFsVersion(versionBuffer, sizeof(versionBuffer));
    strcpy(appConfig.versionFs, versionBuffer);

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
    appConfig.rtsb = pref.getInt("rtsb", PREF_RTSB);
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
    initFs();

    // Initialize application state
    initState();

    // Initialize LED strip
    setupWled();

    if (!appConfig.wifiSet) {
        logger("WiFi not setup yet, starting AP Mode");
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

    // setup events
    events.onConnect([](AsyncEventSourceClient *client) {
        client->send("hello!", NULL, millis(), 1000);
        logger("Server Sent Events:     ok");
    });

    // Start server
    routing(server);
    server.addHandler(&events);
    server.begin();
    logger("HTTP server:            ok");
    logger(String(appConfig.name) + " is ready!");

    startupAnimation();

    // start mqtt
    if (mqttSetup()) {
        mqttReconnect();
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

    // all the loops
    wifiLoop();
    mqttLoop();
}
