#include <Arduino.h>
#include <LittleFS.h>

struct tm timeinfo;
const size_t MAX_LOG_FILE_SIZE = 50 * 1024;  // 50 KB
extern AppConfig appConfig;


// check the log file size and trim if needed
void checkLogFileSize(const char* fileName) {
    File logFile = LittleFS.open(fileName, "r");
    if (!logFile) {
        Serial.println("Failed to open log file for reading");
        return;
    }

    // Check the current file size
    size_t fileSize = logFile.size();
    logFile.close();

    // If the file exceeds the max size, trim the file
    if (fileSize > MAX_LOG_FILE_SIZE) {
        // Open the log file again for reading to trim data
        logFile = LittleFS.open(fileName, "r");
        String newContent;
        size_t bytesToTrim = fileSize - MAX_LOG_FILE_SIZE;

        // Skip older lines until the file is under size limit
        size_t currentSize = 0;
        while (logFile.available()) {
            String line = logFile.readStringUntil('\n');
            currentSize += line.length() + 1;  // +1 for newline character

            // Once we have skipped enough lines to be under the limit, start storing
            if (currentSize > bytesToTrim) {
                newContent += line + "\n";
            }
        }

        logFile.close();

        // Write the new trimmed content back to the file
        logFile = LittleFS.open(fileName, "w");
        if (logFile) {
            logFile.print(newContent);
            logFile.close();
            Serial.println("Log file trimmed successfully");
        } else {
            Serial.println("Failed to open log file for writing");
        }
    }
}


// log data to serial and file
void logger(String logData) {
    getLocalTime(&timeinfo);

    char timeStringBuff[25];
    strftime(timeStringBuff, sizeof(timeStringBuff), "[%Y-%m-%d %H:%M:%S]", &timeinfo);
    String logMessage = String(timeStringBuff) + " - " + logData;

    Serial.println(logMessage);
    delay(10);

    // if debug logging is false quit
    if (!appConfig.logging) {
        return;
    }

    // logging set to true so log to file
    checkLogFileSize("/log.txt");

    File logFile = LittleFS.open("/log.txt", "a");
    if (!logFile) {
        Serial.println("Failed to open log file");
        return;
    }

    logFile.println(logMessage);
    logFile.close();
}


// delete log file
void deleteLogFile() {
  if (LittleFS.exists("/log.txt")) {
      LittleFS.remove("/log.txt");
  }
}