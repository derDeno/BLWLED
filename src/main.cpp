#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <time.h>
#include <FastLED.h>

#include "config.h"
#include "log.h"
#include "action.h"
#include "webserver.h"
#include "mqtt.h"

AppConfig appConfig;
AsyncWebServer server(80);
AsyncEventSource events("/events");
Preferences pref;

int swState = HIGH;
int lastSwState = HIGH;

const int STARTUP_DELAY_MS = 5000;
const int ANALOG_DELAY_MS = 250;
unsigned long lastDebounceTime = 0;
unsigned const long debounceDelay = 1000;

void initState() {

    pref.begin("deviceSettings", true);

    // check if boardname already set, else set it
    char boardName[12];
    sprintf(boardName, "BLWLED-%d%d%d", random(0, 9), random(0, 9), random(0, 9));
    if (pref.getString("name", "").length() == 0) {
        pref.putString("name", boardName);
    }

    strcpy(appConfig.name, pref.getString("name", boardName).c_str());
    appConfig.wled = pref.getBool("wled", true);
    appConfig.count = pref.getInt("count", 10);
    strcpy(appConfig.order, pref.getString("order", "bgr").c_str());
    appConfig.analog = pref.getBool("analog", false);
    appConfig.mode = pref.getInt("mode", 1);
    appConfig.sw = pref.getBool("sw", true);
    appConfig.action = pref.getInt("action", 1);
    appConfig.logging = pref.getBool("logging", true);
    pref.end();

    pref.begin("printerSettings", true);
    strcpy(appConfig.ip, pref.getString("ip", "").c_str());
    strcpy(appConfig.ac, pref.getString("ac", "").c_str());
    strcpy(appConfig.sn, pref.getString("sn", "").c_str());
    appConfig.rtid = pref.getBool("rtid", true);
    appConfig.rtit = pref.getInt("rtit", 10);
    pref.end();

    /*
    pref.begin("wifi", true);
    pref.getBool("setup", false);
    pref.getString("ssid", "");
    pref.getString("pw", "");
    pref.end();
    */
}

void initWifi() {
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
        logger("... ", false);
    }

    // NTP
    configTime(0, 0, "pool.ntp.org");
    logger("NTP time is synchronized. Time is in UTC");

    logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
    logger(String("BLWLED Hostname: ") + String(WiFi.getHostname()));
    logger("RSSI: " + String(WiFi.RSSI()));
}

void startupAnimation(void* pvParameters) {

    // wled animation if active
    if (appConfig.wled && appConfig.count > 0) {
        CRGB leds[appConfig.count];
        FastLED.addLeds<WS2812, WLED_PIN, BGR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);
        FastLED.clear(true);
        FastLED.setBrightness(255);

        for (int i = 0; i < 255; i++) {
            uint8_t hue = beatsin8(10, 255);
            fill_rainbow(leds, appConfig.count, hue, 10);
            FastLED.show();
        }

        vTaskDelay(STARTUP_DELAY_MS / portTICK_PERIOD_MS);
        FastLED.clear(true);
    }

    if (appConfig.analog) {
        analogWrite(ANALOG_PIN_R, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_R, 0);

        analogWrite(ANALOG_PIN_G, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_G, 0);

        analogWrite(ANALOG_PIN_B, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_B, 0);

        analogWrite(ANALOG_PIN_B, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_B, 0);

        analogWrite(ANALOG_PIN_WW, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_WW, 0);

        analogWrite(ANALOG_PIN_CW, 255);
        vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
        analogWrite(ANALOG_PIN_CW, 0);
    }

    // Delete the task after execution
    vTaskDelete(NULL);
}

void setup() {
    bool wifiSet = true;

    Serial.begin(74880);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }
    logger("=============================");
    logger("LittleFS mounted successfully", false);

    // Initialize application state
    initState();

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

    // pins def
    pinMode(SW_PIN, INPUT_PULLUP);
    pinMode(ANALOG_PIN_R, OUTPUT);
    pinMode(ANALOG_PIN_G, OUTPUT);
    pinMode(ANALOG_PIN_B, OUTPUT);
    pinMode(ANALOG_PIN_CW, OUTPUT);
    pinMode(ANALOG_PIN_WW, OUTPUT);
    pinMode(WLED_PIN, OUTPUT);

    // startupAnimation();
    xTaskCreate(startupAnimation, "LED Animation", 4096, NULL, 1, NULL);

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
    logger(String(appConfig.name) + " is ready!");

}

void loop() {
    // react to switch press
    int reading = digitalRead(5);
    if (reading != lastSwState) {
        lastDebounceTime = millis();

        if (reading == LOW) {
            if (appConfig.sw) {
                if (appConfig.action == 1) {
                    actionMaintenance();
                } else if (appConfig.action == 2) {
                    ESP.restart();
                }
            }
        }
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        lastDebounceTime = millis();
    }

    lastSwState = reading;
}
