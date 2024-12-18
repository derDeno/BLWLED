#include <Arduino.h>
#include <FastLED.h>

// System Defenition
#define VERSION "0.1.3"

// Pins Defenition
#define WLED_PIN 18
#define ANALOG_PIN_R 17
#define ANALOG_PIN_G 16
#define ANALOG_PIN_B 4
#define ANALOG_PIN_WW 15
#define ANALOG_PIN_CW 2
#define SW_PIN 5

// Default Pref values
#define PREF_WLED true
#define PREF_COUNT 79
#define PREF_ORDER "grb"
#define PREF_ANALOG false
#define PREF_MODE 1
#define PREF_SW true
#define PREF_ACTION 1
#define PREF_LOGGING false
#define PREF_RTID true
#define PREF_RTSB 600

// Dev defaults
#define PREF_IP "192.168.1.211"
#define PREF_AC "a1452429"
#define PREF_SN "00M09C421800311"
#define PREF_SSID "Unbekannt"
#define PREF_PASS "ffYkexQAETVIb"

/**
 * Application Configuration
 */
struct AppConfig {
    
    // Constructor set defaults
    AppConfig() : version(VERSION), versionFs("n/a"), name("BLWLED"), wled(false), count(0), analog(false), mode(1), sw(true), action(1), logging(false), rtid(true), rtsb(10), wifiSet(false), printerSet(false) {
        strcpy(order, "");
        strcpy(ip, "");
        strcpy(ac, "");
        strcpy(sn, "");
        strcpy(ssid, "");
        strcpy(pass, "");
    }

    // system
    const char version[13];     // version of the firmware
    char versionFs[13];   // version of the filesystem
    char name[14];

    // device states
    bool wled;          // wled active
    uint8_t count;      // number of leds
    char order[4];      // led order
    bool analog;        // analog led active
    uint8_t mode;       // 1 = strip, 2 = individual
    bool sw;            // switch active
    uint8_t action;     // 1 = maintenance, 2 = reboot, 3 = disco
    bool logging;       // logging active

    // printer states
    bool printerSet;    // Was printer already set up
    char ip[16];        // printer ip
    char ac[16];        // access code
    char sn[16];        // serial number
    bool rtid;          // return to idle after door opened while print finished
    uint8_t rtsb;       // return to standby time in seconds

    // WiFi settings
    bool wifiSet;       // Was WiFi already set up
    char ssid[33];      // WiFi ssid
    char pass[64];      // WiFi password
};