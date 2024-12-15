#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClientSecure wifiClient;
PubSubClient mqttClient;

// previous printer state
bool pLightOn = false;
bool pDoorOpen = false;
char pGcodeState[10] = "";
int pStgCur = -99;
int printFinishedTime = -1;
int printIdleTime = -1;


void mqttListen(char* topic, byte* payload, unsigned int length) {

    // parse message
    JsonDocument message;
    JsonDocument filter;
    filter["print"]["command"] =  true;
    filter["print"]["fail_reason"] =  true;
    filter["print"]["gcode_state"] =  true;
    filter["print"]["print_gcode_action"] =  true;
    filter["print"]["print_real_action"] =  true;
    filter["print"]["hms"] =  true;
    filter["print"]["home_flag"] =  true;
    filter["print"]["lights_report"] =  true;
    filter["print"]["stg_cur"] =  true;
    filter["print"]["print_error"] =  true;
    filter["print"]["wifi_signal"] =  true;
    filter["system"]["command"] =  true;
    filter["system"]["led_mode"] =  true;

    DeserializationError error = deserializeJson(message, payload, length, DeserializationOption::Filter(filter));
    
    if(error) {
        logger("E:  Failed to parse message");
        return;
    }

    const char* print_gcode_state = message["print"]["gcode_state"];
    int print_stg_cur = message["print"]["stg_cur"];

    // parse home flag
    long home_flag = message["print"]["home_flag"];
    uint32_t DOOR_OPEN = 0x00800000; // const for door open flag
    bool doorOpen = false;
    if ((home_flag & DOOR_OPEN) == DOOR_OPEN) {
        doorOpen = true;
    }

    // parse light state
    bool lightOn = false;
    for (JsonObject print_lights_report_item : message["print"]["lights_report"].as<JsonArray>()) {

        // check for the chamber light node
        if(print_lights_report_item["node"] == "chamber_light") {
            const char* light = print_lights_report_item["mode"];
            if(strcmp(light, "on") == 0) {
                lightOn = true;
            }
        }
    }

    // check for gcode state change
    switch (print_stg_cur) {
        case -1:
            if(strcmp(print_gcode_state, "IDLE") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                printIdleTime = millis();
                eventBus(EVENT_PRINTER_IDLE);

            } else if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                // printer sent print file - no event specified yet

            } else if(strcmp(print_gcode_state, "FINISH") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                printFinishedTime = millis();
                eventBus(EVENT_PRINT_FINISHED);
            }
            break;
        case 0:
            if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                eventBus(EVENT_PRINTING);
            }
            break;
        case 1:
            if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                eventBus(EVENT_BED_LEVELING);
            }
            break;
        case 2:
            if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                eventBus(EVENT_PREHEAT_BED);
            }
            break;
        case 8:
            if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                eventBus(EVENT_EXTRUSION_CALIBRATION);
            }
            break;
        case 14:
            if(strcmp(print_gcode_state, "RUNNING") == 0 && strcmp(pGcodeState, print_gcode_state) != 0) {
                eventBus(EVENT_CLEANING_NOZZLE);
            }
            break;
        default:
            break;
    }


    // door state change while idle
    if(pStgCur == -1 && print_stg_cur == -1 && strcmp(print_gcode_state, "IDLE") == 0) {
        if(pDoorOpen != doorOpen) {
            if(doorOpen) {
                eventBus(EVENT_DOOR_OPEN_IDLE);
            } else {
                eventBus(EVENT_DOOR_CLOSE_IDLE);
            }
        }
    }

    // door state change while printing
    if(pStgCur == 0 && print_stg_cur == 0 && strcmp(print_gcode_state, "RUNNING") == 0) {
        if(pDoorOpen != doorOpen) {
            if(doorOpen) {
                eventBus(EVENT_DOOR_OPEN_PRINT);
            } else {
                eventBus(EVENT_DOOR_CLOSE_PRINT);
            }
        }
    }

    // door state change while finish
    if(pStgCur == -1 && print_stg_cur == -1 && strcmp(print_gcode_state, "FINISH") == 0) {
        if(pDoorOpen != doorOpen) {
            if(doorOpen) {
                eventBus(EVENT_DOOR_OPEN_FINISH);
            } else {
                eventBus(EVENT_DOOR_CLOSE_FINISH);
            }
        }
    }

    // light state change
    if(pLightOn != lightOn) {
        if(lightOn) {
            eventBus(EVENT_LIGHT_ON);
        } else {
            eventBus(EVENT_LIGHT_OFF);
        }
    }


    // check appconfig if return to idle after door open is enabled and print is finished
    if(appConfig.rtid && printFinishedTime != -1) {
        if(pDoorOpen != doorOpen) {
            if(doorOpen) {
                printFinishedTime = -1;
                printIdleTime = millis();
                eventBus(EVENT_PRINTER_IDLE);
            }
        }
    }


    // check appconfig if return to standby is enabled and if so, when to return (time) after print finished or idle
    if(printFinishedTime != -1 && appConfig.rtsb > 0) {
        if((millis() - printFinishedTime) > (appConfig.rtsb * 1000)) {
            printFinishedTime = -1;
            eventBus(EVENT_PRINTER_STANDBY);
        }
    }else if(printIdleTime != -1 && appConfig.rtsb > 0) {
        if((millis() - printIdleTime) > (appConfig.rtsb * 1000)) {
            printIdleTime = -1;
            eventBus(EVENT_PRINTER_STANDBY);
        }
    }


    // save current state
    pLightOn = lightOn;
    pDoorOpen = doorOpen;
    pStgCur = print_stg_cur;
    strcpy(pGcodeState, print_gcode_state);
}


int mqttReconnect() {
    
    // check if wifi is connected
    if (WiFi.status() != WL_CONNECTED) {
        return 0;
    }

    // if mqtt connected, disconnect first
    if(mqttClient.connected()) {
        mqttClient.disconnect();
    }

    int retries = 0;
    while (!mqttClient.connected() && retries < 5) {

        if (mqttClient.connect(appConfig.name, "bblp", appConfig.ac)) {
            logger("MQTT connected to printer");

            String topic = String("device/") + appConfig.sn + String("/report");
            if(mqttClient.subscribe(topic.c_str())) {
                logger("Subscribed to " + topic);
            } else {
                logger("E:  Failed to subscribe to " + topic);
            }
            return 1;

        } else {
            logger("E:  MQTT failed with state " + String(mqttClient.state()));
            delay(2000);
            retries++;
        }
    }

    logger("E:  Failed to connect to MQTT after 5 retries");
    return 0;
}


bool mqttSetup() {

    if (!appConfig.printerSet) {
        logger("E:  Printer not configured!");
        return false;
    }

    wifiClient.setInsecure();

    mqttClient.setClient(wifiClient);
    mqttClient.setBufferSize(12000);
    mqttClient.setServer(appConfig.ip, 8883);
    mqttClient.setCallback(mqttListen);
    mqttClient.setSocketTimeout(20);

    return true;
}


void mqttLoop() {
    if (!mqttClient.connected()) {
        mqttReconnect();
    }
    mqttClient.loop();
}

// TODO: Add HMS error handling and print error handling