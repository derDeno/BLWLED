#include <Arduino.h>

#define WLED_PIN 18
#define ANALOG_PIN_R 17
#define ANALOG_PIN_G 16
#define ANALOG_PIN_B 4
#define ANALOG_PIN_WW 15
#define ANALOG_PIN_CW 2
#define SW_PIN 5


struct AppConfig {
    
    // Constructor
    AppConfig() : version("0.1.1-b"), name("BLWLED"), wled(false), count(0), analog(false), mode(1), sw(false), action(1), logging(true), rtid(true), rtit(10) {
        strcpy(order, "");
        strcpy(ip, "");
        strcpy(ac, "");
        strcpy(sn, "");
    }

    // system
    const char version[8];
    char name[12];

    // device states
    bool wled;          // wled active
    uint8_t count;      // number of leds
    char order[4];      // led order
    bool analog;        // analog led active
    uint8_t mode;       // 1 = strip, 2 = individual
    bool sw;            // switch active
    uint8_t action;       // 1 = event, 2 = reboot
    bool logging;       // logging active

    // printer states
    char ip[16];        // printer ip
    char ac[16];        // access code
    char sn[16];        // serial number
    bool rtid;          // return to idle after door opened
    uint8_t rtit;       // return to idle time in seconds
};