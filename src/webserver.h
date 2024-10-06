#include <ESPAsyncWebserver.h>
#include <LITTLEFS.h>
#include <WiFi.h>
#include <Preferences.h>

#include <log.h>

Preferences pref;

String processorInfo(const String &var) {
    if (var == "TEMPLATE_MAC") {
        return WiFi.macAddress();

    } else if (var == "TEMPLATE_IP") {
        return WiFi.localIP().toString();

    } else if (var == "TEMPLATE_HOSTNAME") {
        return String(WiFi.getHostname());

    } else if (var == "TEMPLATE_RSSI") {
        return String(WiFi.RSSI());

    } else if (var == "TEMPLATE_VERSION") {
        return F("0.0.1-b");

    } else if (var == "TEMPLATE_UPTIME") {
        unsigned long uptimeMillis = millis();

        unsigned long seconds = uptimeMillis / 1000;
        unsigned long minutes = seconds / 60;
        unsigned long hours = minutes / 60;
        unsigned long days = hours / 24;

        seconds = seconds % 60;
        minutes = minutes % 60;
        hours = hours % 24;

        String uptime = String(days) + " days " + String(hours) + "h " + (minutes < 10 ? "0" : "") + String(minutes) + "min " + (seconds < 10 ? "0" : "") + String(seconds) + "s";
        return uptime;

    } else if (var == "TEMPLATE_LOCAL_TIME") {
        return F("to be added");
    }

    return String();
}

String processorLogs(const String &var) {
    if (var == "LOG_TEMPLATE") {
        File logFile = LittleFS.open("/log.txt", "r");
        String logContent = "";
        if (logFile) {
            while (logFile.available()) {
                logContent += logFile.readStringUntil('\n') + "<br>";
            }
            logFile.close();
        } else {
            logContent = "Log file not found!";
        }
        return logContent;
    }

    // Return an empty string if the placeholder is unknown
    return String();
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void routing(AsyncWebServer &server) {
    // Serve static files from LittleFS
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setFilter(ON_STA_FILTER);
    server.serveStatic("/", LittleFS, "/captive.html").setFilter(ON_AP_FILTER);

    // Main routes
    server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/info.html", String(), false, processorInfo);
    });

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/log.html", String(), false, processorLogs);
    });

    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings-device.html", String(), false);
    });

    server.on("/settings-mapping", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings-mapping.html", String(), false);
    });

    server.on("/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings-printer.html", String(), false);
    });

    server.on("/settings-update", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings-update.html", String(), false);
    });

    server.on("/settings-wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings-wifi.html", String(), false);
    });


    // API routes
    server.on("/api/log-download", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (LittleFS.exists("/log.txt")) {
            request->send(LittleFS, "/log.txt", "text/plain", true);
        } else {
            request->send(404, "text/plain", "Log file not found!");
        }
    });

    server.on("/api/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
        logger("Reset by user");

        // clear all settings
        pref.begin("wifi", false);
        pref.clear();

        pref.begin("printer", false);
        pref.clear();

        pref.begin("device", false);
        pref.clear();

        pref.begin("mapping", false);
        pref.clear();

        request->redirect("/");
        delay(100);
        ESP.restart();
    });

    server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
        logger("Reboot by user");
        request->redirect("/");
        delay(100);
        ESP.restart();
    });

    // AP SETTINGS
    server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
        // receive wifi data and connect
        pref.begin("wifi", false);
        pref.putString("ssid", "");

        pref.putString("password", "");
        pref.end();
    });
    
    // MAPPINGS
    server.on("/api/mappings", HTTP_GET, [](AsyncWebServerRequest *request) {
        
    });
    
    server.on("/api/mappings", HTTP_POST, [](AsyncWebServerRequest *request) {
        
    });

    server.on("/api/mappings", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        if(request->hasParam("id")) {
            int id = request->getParam("id")->value().toInt();
        }
    });

    server.on("/api/mapping-upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        
    });

    server.on("/api/mapping-download", HTTP_GET, [](AsyncWebServerRequest *request) {
        
    });

    server.on("/api/test-mapping", HTTP_GET, [](AsyncWebServerRequest *request) {
        if(request->hasParam("id")) {
            int id = request->getParam("id")->value().toInt();
        }
    });

    
    // SETTINGS DEVICE
    server.on("/api/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
        // respond device settings json
    });

    server.on("/api/settings-device", HTTP_POST, [](AsyncWebServerRequest *request) {
        // receive settings device
    });


    // SETTINGS PRINTER
    server.on("/api/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
        // respond printer settings as json
    });

    server.on("/api/settings-printer", HTTP_POST, [](AsyncWebServerRequest *request) {
        // receive printer settings
    });

    server.on("/api/test-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
        // check printer mqtt for info msg
    });


    // BACKUP & OTA
    server.on("/api/backup-download", HTTP_GET, [](AsyncWebServerRequest *request) {
        // generate settings download file
    });

    server.on("/api/backup-upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        // receive settings file and reboot
    });

    server.on("/api/ota-upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        // receive ota file and process
    });


    server.onNotFound(notFound);
}