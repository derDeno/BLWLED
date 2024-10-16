#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <LittleFS.h>

struct tm timeinfo;
const size_t MAX_LOG_FILE_SIZE = 200 * 1024;  // 200 KB

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

void logger(String logData, bool file = true) {
  getLocalTime(&timeinfo);

  char timeStringBuff[25];
  strftime(timeStringBuff, sizeof(timeStringBuff), "[%Y-%m-%d %H:%M:%S]", &timeinfo);
  String logMessage = String(timeStringBuff) + " - " + logData;

  Serial.println(logMessage);
  delay(10);

  if (!file) {
    return;
  }

  checkLogFileSize("/log.txt");

  File logFile = LittleFS.open("/log.txt", "a");
  if (!logFile) {
    Serial.println("Failed to open log file");
    return;
  }

  logFile.println(logMessage);
  logFile.close();
}

#endif