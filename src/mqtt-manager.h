#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern AppConfig appConfig;

WiFiClientSecure wifiClient;
PubSubClient mqttClient;


void mqtt_listen(char* topic, byte* payload, unsigned int length) {
    logger("MQTT received");
    logger(topic);

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

    long home_flag = message["print"]["home_flag"];
    uint32_t DOOR_OPEN = 0x00800000; // const for door open flag
    bool door_open = false;
    if ((home_flag & DOOR_OPEN) == DOOR_OPEN) {
        door_open = true;
    }

    bool light_on = false;
    for (JsonObject print_lights_report_item : message["print"]["lights_report"].as<JsonArray>()) {

        // check for the chamber light node
        if(print_lights_report_item["node"] == "chamber_light") {
            const char* light = print_lights_report_item["mode"];
            if(strcmp(light, "on") == 0) {
                light_on = true;
            }
        }
    }

    // debug print
    Serial.println("Door state: " + String(door_open));
}


int mqtt_reconnect() {
    
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


bool mqtt_setup() {

    if (!appConfig.printerSet) {
        logger("E:  Printer not configured!");
        return false;
    }

    wifiClient.setInsecure();

    mqttClient.setClient(wifiClient);
    mqttClient.setBufferSize(6000);
    mqttClient.setServer(appConfig.ip, 8883);
    mqttClient.setCallback(mqtt_listen);
    mqttClient.setSocketTimeout(20);

    return true;
}


void mqtt_loop() {
    if (!mqttClient.connected()) {
        mqtt_reconnect();
    }
    mqttClient.loop();
}

//Thanks to DutchDevelop BLLED Project for the following information
//Expected information when viewing MQTT status messages
			
//gcode_state	stg_cur	    BLLED LED control	    Comments
//------------------------------------------------------------------------
//IDLE	        -1	        White	                Printer just powered on
//RUNNING	    -1	        White	                Printer sent print file
//RUNNING	     2	        White	                PREHEATING BED
//RUNNING	    14	        OFF (for Lidar)	        CLEANING NOZZLE
//RUNNING	     1	        OFF (for Lidar)	        BED LEVELING
//RUNNING	     8	        OFF (for Lidar)	        CALIBRATING EXTRUSION
//RUNNING	     0	        White	                All the printing happens here
//FINISH	    -1	        Green	                After bed is lowered and filament retracted
//FINISH	    -1	        Green	                BLLED logic waits for a door interaction
//FINISH	    -1	        White	                After door interaction
//FINISH	    -1	        OFF                     Inactivity after 30mins