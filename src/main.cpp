#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <time.h>
#include <FastLED.h>

#include "state.h"
#include "action.h"
#include "webserver.h"

AppState appState;
AsyncWebServer server(80);
Preferences pref;

int swState = HIGH;
int lastSwState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;

void initState() {
    pref.begin("deviceSettings", true);
    appState.wled = pref.getBool("wled", true);
    appState.count = pref.getInt("count", 10);
    strcpy(appState.order, pref.getString("order", "gbr").c_str());
    appState.analog = pref.getBool("analog", false);
    appState.mode = pref.getInt("mode", 1);
    appState.sw = pref.getBool("sw", false);
    appState.fnct = pref.getInt("function", 1);
    appState.logging = pref.getBool("logging", true);
    pref.end();

    pref.begin("printerSettings", true);
    strcpy(appState.ip, pref.getString("ip", "").c_str());
    strcpy(appState.ac, pref.getString("ac", "").c_str());
    strcpy(appState.sn, pref.getString("sn", "").c_str());
    appState.rtid = pref.getBool("rtid", true);
    appState.rtit = pref.getInt("rtit", 10);
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
    configTime(0, 0, "pool.ntp.org");
    logger("NTP time is synchronized. Time is in UTC");

    logger("Connected to WiFi network with IP Address: " + WiFi.localIP().toString());
    logger(String("BLWLED Hostname: ") + String(WiFi.getHostname()));
    logger("RSSI: " + String(WiFi.RSSI()));
}

const int WLED_PIN = 18;
const int ANALOG_PINS[] = {17, 16, 4, 15, 2};
const int STARTUP_DELAY_MS = 3000;
const int ANALOG_DELAY_MS = 250;

void startupAnimation(void* pvParameters) {
    Serial.println("Startup Animation");

    const char* order = appState.order;
    const int mode = appState.mode;

    // wled animation if active
    if (appState.wled && appState.count > 0) {
        static CRGB leds[100]; // Allocate a fixed-size array to avoid dynamic allocation
        FastLED.addLeds<WS2812, WLED_PIN, GBR>(leds, appState.count).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(255);

        // fill_solid(leds, wledPixel, CRGB::White);
        fill_rainbow(leds, appState.count, 0, 30);
        FastLED.show();
        vTaskDelay(STARTUP_DELAY_MS / portTICK_PERIOD_MS);
        FastLED.clear(true);
    }

    if (appState.analog) {
        for (int i = 0; i < sizeof(ANALOG_PINS) / sizeof(ANALOG_PINS[0]); i++) {
            analogWrite(ANALOG_PINS[i], 255);
            vTaskDelay(ANALOG_DELAY_MS / portTICK_PERIOD_MS);
            analogWrite(ANALOG_PINS[i], 0);
        }
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
    pinMode(5, INPUT);
    pinMode(17, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(18, OUTPUT);

    // startupAnimation();
    xTaskCreate(startupAnimation, "LED Animation", 1000, NULL, 1, NULL);

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
}

void loop() {
    // react to switch press
    int reading = digitalRead(5);
    if (reading != lastSwState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != swState) {
            swState = reading;

            if (swState == LOW) {
                logger("Onboard switch pressed");
                
                bool swActive = appState.sw;
                int action = appState.fnct;

                if (swActive) {
                    if (action == 1) {
                        actionMaintenance();
                    } else if (action == 2) {
                        ESP.restart();
                    }
                }
            }
        }
    }
}
