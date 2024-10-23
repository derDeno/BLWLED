#include <ESPAsyncWebServer.h>
#include <time.h>

#include "action.h"
#include "state.h"
#include "webserver.h"

const char* ssid = "Unbekannt";
const char* password = "ffYkexQAETVIb";
bool wifiSet = true;

AppState appState;
AsyncWebServer server(80);

int swState = HIGH;
int lastSwState = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 1000;

void initState() {
    pref.begin("deviceSettings", true);
    appState.wled = pref.getBool("wled", true);
    appState.count = pref.getInt("count", 10);
    appState.order = strcpy(pref.getChar("order", "gbr"));
    appState.analog = pref.getBool("analog", false);
    appState.mode = pref.getInt("mode", 1);
    appState.sw = pref.getBool("sw", false);
    appState.fnct = pref.getInt("function", 1);
    appState.logging = pref.getBool("logging", true);
    pref.end();

    pref.begin("printerSettings", true);
    appState.ip = pref.getChar("ip", *"");
    appState.ac = pref.getChar("ac", *"");
    appState.sn = pref.getChar("sn", *"");
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

void startupAnimation(void* pvParameters) {
    Serial.println("Startup Animation");

    const char* order = appState.order;
    const int mode = appState.mode;

    // wled animation if active
    if (appState.wled) {
        CRGB leds[appState.count];

        FastLED.addLeds<WS2812, 18, GBR>(leds, appState.count).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(255);

        // fill_solid(leds, wledPixel, CRGB::White);
        fill_rainbow(leds, appState.count, 0, 30);
        FastLED.show();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        FastLED.clear(true);
    }

    if (appState.analog) {
        int pins[] = {17, 16, 4, 15, 2};
        for (int i = 0; i < 5; i++) {
            analogWrite(pins[i], 255);
            vTaskDelay(250 / portTICK_PERIOD_MS);
            analogWrite(pins[i], 0);
        }
    }
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
