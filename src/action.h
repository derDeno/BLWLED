#include <Arduino.h>

// Pin Control action. Perform action defined in mapping
void actionPinControl(String pin, String control, bool invert = false, int invert_delay = 0, int flash_timout = 0, int flash_count = 0) {

    int outputPin = outputToPin(pin);

    // first check if option is set to flash
    if(control == "flash") {
        for(int i = 0; i < flash_count; i++) {
            digitalWrite(outputPin, HIGH);
            delay(flash_timout * 1000);
            digitalWrite(outputPin, LOW);
            delay(flash_timout * 1000);
        }
    } else if (control == "on") {
        digitalWrite(outputPin, HIGH);
        if(invert) {
            delay(invert_delay * 1000);
            digitalWrite(outputPin, LOW);
        }
    } else if (control == "off") {
        digitalWrite(outputPin, LOW);
        if(invert) {
            delay(invert_delay * 1000);
            digitalWrite(outputPin, HIGH);
        }
    }
}

// RGB Control action. Perform action defined in mapping



// convert output names to actual pin numbers
int outputToPin(String output) {
    if (output == "analog-r") {
        return 0;
    }else if (output == "analog-g") {
        return 0;
    }else if (output == "analog-b") {
        return 0;
    }else if (output == "analog-ww") {
        return 0;
    }else if (output == "analog-cw") {
        return 0;
    }else {
        // default
        return output.toInt();
    }
}