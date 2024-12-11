#include <Arduino.h>

extern AppConfig appConfig;
extern CRGB *leds;

bool maintenanceToggle = false;

typedef enum {
    EVENT_SW_CLICK,
    EVENT_PRINTER_IDLE,
    EVENT_PREHEAT_BED,
    EVENT_CLEANING_NOZZLE,
    EVENT_BED_LEVELING,
    EVENT_EXTRUSION_CALIBRATION,
    EVENT_PRINTING,
    EVENT_PRINT_FINISHED,
    EVENT_PRINT_FAILED,
    EVENT_DOOR_OPEN_IDLE,
    EVENT_DOOR_CLOSE_IDLE,
    EVENT_DOOR_OPEN_PRINT,
    EVENT_DOOR_CLOSE_PRINT,
    EVENT_DOOR_OPEN_FINISH,
    EVENT_DOOR_CLOSE_FINISH
} EventType;

void eventMaintenance() {
    if (!maintenanceToggle) {
        if (appConfig.wled) {
            actionColorWled("#ffffff", 125);

        } else if (appConfig.analog) {
            if (appConfig.mode == 1) {
                actionColor("#ffffff", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 255);
            }
        }

        maintenanceToggle = true;
    } else {
        if (appConfig.wled) {
            FastLED.clear(true);

        } else if (appConfig.analog) {
            if (appConfig.mode == 1) {
                actionColor("#000000", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 0);
            }
        }

        maintenanceToggle = false;
    }
}


void eventBus(EventType event) {
    switch (event) {
        case EVENT_SW_CLICK:
            if (appConfig.sw) {
                if (appConfig.action == 1) {
                    eventMaintenance();
                } else if (appConfig.action == 2) {
                    ESP.restart();
                }
            }
            
            break;
        case EVENT_PRINTER_IDLE:
            break;
        case EVENT_PREHEAT_BED:
            break;
        case EVENT_CLEANING_NOZZLE:
            break;
        case EVENT_BED_LEVELING:
            break;
        case EVENT_EXTRUSION_CALIBRATION:
            break;
        case EVENT_PRINTING:
            break;
        case EVENT_PRINT_FINISHED:
            break;
        case EVENT_PRINT_FAILED:
            break;
        case EVENT_DOOR_OPEN_IDLE:
            break;
        case EVENT_DOOR_CLOSE_IDLE:
            break;
        case EVENT_DOOR_OPEN_PRINT:
            break;
        case EVENT_DOOR_CLOSE_PRINT:
            break;
        case EVENT_DOOR_OPEN_FINISH:
            break;
        case EVENT_DOOR_CLOSE_FINISH:
            break;
    }
}