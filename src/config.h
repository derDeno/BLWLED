#include <Arduino.h>

#define WLED_PIN 18
#define ANALOG_PIN_R 17
#define ANALOG_PIN_G 16
#define ANALOG_PIN_B 4
#define ANALOG_PIN_WW 15
#define ANALOG_PIN_CW 2
#define SW_PIN 5

extern Preferences pref;

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

    // check key and updata param and also update corresponding pref
    void updateDevice(const char* key, void* param) {
        pref.begin("deviceSettings");

        if(strcmp(key, "name") == 0) {
            strcpy(name, (char*)param);
            pref.putString("name", (char*)param);

    	}else if(strcmp(key, "wled") == 0) {
            wled = (bool)param;
            pref.putBool("wled", (bool)param);

        }else if(strcmp(key, "count") == 0) {
            count = (uint8_t)param;
            pref.putInt("count", (uint8_t)param);

        }else if(strcmp(key, "order") == 0) {
            strcpy(order, (char*)param);
            pref.putString("order", (char*)param);

        }else if(strcmp(key, "analog") == 0) {
            analog = (bool)param;
            pref.putBool("analog", (bool)param);

        }else if(strcmp(key, "mode") == 0) {
            mode = (uint8_t)param;
            pref.putInt("mode", (uint8_t)param);

        }else if(strcmp(key, "sw") == 0) {
            sw = (bool)param;
            pref.putBool("sw", (bool)param);

        }else if(strcmp(key, "action") == 0) {
            action = (uint8_t)param;
            pref.putInt("action", (uint8_t)param);

        }else if(strcmp(key, "logging") == 0) {
            logging = (bool)param;
            pref.putBool("logging", (bool)param);

        }

        pref.end();
    }

    void updatePrinter(const char* key, void* param) {
        pref.begin("printerSettings");

        if(strcmp(key, "ip") == 0) {
            strcpy(ip, (char*)param);
            pref.putString("ip", (char*)param);

        }else if(strcmp(key, "ac") == 0) {
            strcpy(ac, (char*)param);
            pref.putString("ac", (char*)param);

        }else if(strcmp(key, "sn") == 0) {
            strcpy(sn, (char*)param);
            pref.putString("sn", (char*)param);

        }else if(strcmp(key, "rtid") == 0) {
            rtid = (bool)param;
            pref.putBool("rtid", (bool)param);

        }else if(strcmp(key, "rtit") == 0) {
            rtit = (uint8_t)param;
            pref.putInt("rtit", (uint8_t)param);
        }

        pref.end();
    }
};