#include <Arduino.h>

struct AppState {
    // system
    const char version[8] = "0.1.1-b";

    // device states
    bool wled;          // wled active
    uint8_t count;      // number of leds
    char order[4];      // led order
    bool analog;        // analog led active
    uint8_t mode;       // 1 = strip, 2 = individual
    bool sw;            // switch active
    uint8_t fnct;       // 1 = event, 2 = reboot
    bool logging;       // logging active

    // printer states
    char ip[16];        // printer ip
    char ac[16];        // access code
    char sn[16];        // serial number
    bool rtid;          // return to idle after door opened
    uint8_t rtit;       // return to idle time in seconds    
};