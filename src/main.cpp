#include <ESPAsyncWebServer.h>
#include <time.h>

#include "state.h"
#include "action.h"
#include "webserver.h"


String ssid = "Unbekannt";
String password = "ffYkexQAETVIb";
bool wifiSet = true;

AppState appState;
AsyncWebServer server(80);

void initState() {
    pref.begin("deviceSettings", true);
    appState.wled = pref.getBool("wled", true);
    appState.count = pref.getInt("count", 10);
    appState.order = pref.getChar("order", "gbr");
    appState.analog = pref.getBool("analog", false);
    appState.mode = pref.getInt("mode", 1);
    appState.sw = pref.getBool("sw", false);
    appState.fnct = pref.getInt("function", 1);
    appState.logging = pref.getBool("logging", true);
    pref.end();

    pref.begin("printerSettings", true);
    appState.ip = pref.getChar("ip", "");
    appState.ac = pref.getString("ac", "");
    appState.sn = pref.getString("sn", "");
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

void startupAnimation() {

    logger("Startup Animation");

    
    const int wledPixel = appState.count;
    const char order = appState.order;
    const int mode = appState.mode;
    const bool wled = appState.wled;
    const bool analog = appState.analog;

    // wled animation if active
    if (wled) {
        CRGB leds[wledPixel];

        FastLED.addLeds<WS2812, 18, GBR>(leds, wledPixel).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(255);

        // fill_solid(leds, wledPixel, CRGB::White);
        fill_rainbow(leds, wledPixel, 0, 30);
        FastLED.show();
        delay(3000);
        FastLED.clear(true);
    }

    if (analog) {
        int pins[] = {17, 16, 4, 15, 2};
        for (int i = 0; i < 5; i++) {
            analogWrite(pins[i], 255);
            delay(250);
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

    startupAnimation();

    // Start server
    routing(server);
    server.begin();
    logger("HTTP server started");
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

        if (swActive) {
            if (action == "event") {
                actionMaintenance();
            } else if (action == "reboot") {
                ESP.restart();
            }
        }
        delay(1000);  // debounce
    }
}
