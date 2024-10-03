#include <ESPAsyncWebserver.h>
#include <LITTLEFS.h>
#include <WiFi.h>

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

    // get all mappings
    server.on("/api/mappings", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    // add mapping
    server.on("/api/mappings", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    // delete mapping
    server.on("/api/mappings", HTTP_DELETE, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });


    server.on("/api/reset-device", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/mapping-upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/mapping-download", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/settings-device", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/settings-device", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/settings-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/settings-printer", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/test-mapping", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/test-printer", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/backup-download", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.on("/api/backup-upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/settings.html", String(), false);
    });

    server.onNotFound(notFound);
}