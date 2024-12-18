#include <Arduino.h>
#include <LittleFS.h>


/**
 * Read the version from the filesystem
 * @param versionBuffer     Buffer to store the version
 * @param bufferSize        Size of the buffer
 */
void readFsVersion(char* versionBuffer, size_t bufferSize) {
    File versionFile = LittleFS.open("/version.txt", "r");
    if (!versionFile) {
        Serial.println("Failed to open version file");
        versionBuffer[0] = '\0';
        return;
    }

    // Read characters until newline or end of file
    size_t index = 0;
    while (index < bufferSize - 1 && versionFile.available()) {
        char c = versionFile.read();
        if (c == '\n' || c == '\r') {
            break;
        }
        versionBuffer[index++] = c;
    }

    versionBuffer[index] = '\0';
    versionFile.close();
}

/**
 * Initialize the filesystem
 */
void initFs() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS mount failed, formatting...");
        LittleFS.format();

    } else if (!LittleFS.exists("/version.txt")) {
        Serial.println("Version file missing");

    } else {
        Serial.println("Filesystem mounted.");
    }
}