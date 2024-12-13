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
    EVENT_DOOR_CLOSE_FINISH,
    EVENT_LIGHT_ON,
    EVENT_LIGHT_OFF,
    EVENT_PRINTER_STANDBY
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
            logger("Event: SW Click");
            if (appConfig.sw) {
                if (appConfig.action == 1) {
                    eventMaintenance();
                } else if (appConfig.action == 2) {
                    ESP.restart();
                }
            }
            break;
        case EVENT_PRINTER_IDLE:
            logger("Event: Printer Idle");
            break;
        case EVENT_PREHEAT_BED:
            logger("Event: Preheat Bed");
            break;
        case EVENT_CLEANING_NOZZLE:
            logger("Event: Cleaning Nozzle");
            FastLED.clear(true);
            break;
        case EVENT_BED_LEVELING:
            logger("Event: Bed Leveling");
            FastLED.clear(true);
            break;
        case EVENT_EXTRUSION_CALIBRATION:
            logger("Event: Extrusion Calibration");
            FastLED.clear(true);
            break;
        case EVENT_PRINTING:
            logger("Event: Printing");
            actionColorWled("#ffffff", 50);
            break;
        case EVENT_PRINT_FINISHED:
            logger("Event: Print Finished");
            actionColorWled("#00ff00", 125);
            break;
        case EVENT_PRINT_FAILED:
            logger("Event: Print Failed");
            break;
        case EVENT_DOOR_OPEN_IDLE:
            logger("Event: Door Open Idle");
            actionColorWled("#ffffff", 255);
            break;
        case EVENT_DOOR_CLOSE_IDLE:
            logger("Event: Door Close Idle");
            actionColorWled("#ffffff", 50);
            break;
        case EVENT_DOOR_OPEN_PRINT:
            logger("Event: Door Open Print");
            break;
        case EVENT_DOOR_CLOSE_PRINT:
            logger("Event: Door Close Print");
            break;
        case EVENT_DOOR_OPEN_FINISH:
            logger("Event: Door Open Finish");
            actionColorWled("#ffffff", 255);
            break;
        case EVENT_DOOR_CLOSE_FINISH:
            logger("Event: Door Close Finish");
            FastLED.clear(true);
            break;
        case EVENT_LIGHT_ON:
            logger("Event: Light On");
            actionColorWled("#ffffff", 125);
            break;
        case EVENT_LIGHT_OFF:
            logger("Event: Light Off");
            FastLED.clear(true);
            break;
        case EVENT_PRINTER_STANDBY:
            logger("Event: Printer Standby");
            FastLED.clear(true);
            break;
        default:
            break;
    }
}