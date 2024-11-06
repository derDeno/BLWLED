#include <Arduino.h>
#include <FastLED.h>

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
#define PREF_COUNT 10
#define PREF_ORDER "grb"
#define PREF_ANALOG false
#define PREF_MODE 1
#define PREF_SW true
#define PREF_ACTION 1
#define PREF_LOGGING true
#define PREF_RTID true
#define PREF_RTIT 10

struct AppConfig {
    
    // Constructor set defaults
    AppConfig() : version("0.1.1-b"), name("BLWLED"), wled(false), count(0), analog(false), mode(1), sw(true), action(1), logging(true), rtid(true), rtit(10) {
        strcpy(order, "");
        strcpy(ip, "");
        strcpy(ac, "");
        strcpy(sn, "");
    }

    // system
    const char version[8];
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
    char ip[16];        // printer ip
    char ac[16];        // access code
    char sn[16];        // serial number
    bool rtid;          // return to idle after door opened
    uint8_t rtit;       // return to idle time in seconds
};