#include <Arduino.h>
#include <FastLED.h>
#include <Preferences.h>

extern AppConfig appConfig;
bool maintenancenToggle = false;

/**
 * Helper Functions
 */

int outputToPin(const char* output) {
    if (strcmp(output, "analog-r") == 0) {
        return ANALOG_PIN_R;
    } else if (strcmp(output, "analog-g") == 0) {
        return ANALOG_PIN_G;
    } else if (strcmp(output, "analog-b") == 0) {
        return ANALOG_PIN_B;
    } else if (strcmp(output, "analog-ww") == 0) {
        return ANALOG_PIN_WW;
    } else if (strcmp(output, "analog-cw") == 0) {
        return ANALOG_PIN_CW;
    } else {
        // default
        return atoi(output);
    }
}

void hexToRgb(String hexColor, uint8_t &r, uint8_t &g, uint8_t &b) {
    // Remove the "#" if present
    if (hexColor.charAt(0) == '#') {
        hexColor = hexColor.substring(1);
    }

    // Convert the hex substring to decimal values
    r = strtoul(hexColor.substring(0, 2).c_str(), NULL, 16);  // Red
    g = strtoul(hexColor.substring(2, 4).c_str(), NULL, 16);  // Green
    b = strtoul(hexColor.substring(4, 6).c_str(), NULL, 16);  // Blue
}


/**
 * Actions
 */

// Pin Control action. Perform action defined in mapping
void actionPinControl(const char* pin, String control, bool invert = false, int invert_delay = 0, int flash_timout = 0, int flash_count = 0) {
    int outputPin = outputToPin(pin);

    // first check if option is set to flash
    if (control == "flash") {
        for (int i = 0; i < flash_count; i++) {
            digitalWrite(outputPin, HIGH);
            delay(flash_timout * 1000);
            digitalWrite(outputPin, LOW);
            delay(flash_timout * 1000);
        }
    } else if (control == "on") {
        digitalWrite(outputPin, HIGH);
        if (invert) {
            delay(invert_delay * 1000);
            digitalWrite(outputPin, LOW);
        }
    } else if (control == "off") {
        digitalWrite(outputPin, LOW);
        if (invert) {
            delay(invert_delay * 1000);
            digitalWrite(outputPin, HIGH);
        }
    }
}

// WLED Control action. Perform action defined in mapping
void actionColorWled(const char* color, int brightness = 255, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {

    CRGB leds[appConfig.count];
    uint8_t r, g, b;
    hexToRgb(color, r, g, b);

    if (String(appConfig.order).equalsIgnoreCase("rgb")) {
        Serial.println("i am rgb");
        FastLED.addLeds<WS2812, WLED_PIN, RGB>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("rbg")) {
        Serial.println("i am rbg");
        FastLED.addLeds<WS2812, WLED_PIN, RBG>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("brg")) {
        Serial.println("i am brg");
        FastLED.addLeds<WS2812, WLED_PIN, BRG>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("bgr")) {
        Serial.println("i am bgr");
        FastLED.addLeds<WS2812, WLED_PIN, BGR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("grb")) {
        Serial.println("i am grb");
        FastLED.addLeds<WS2812, WLED_PIN, GRB>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("gbr")) {
        Serial.println("i am gbr");
        FastLED.addLeds<WS2812, WLED_PIN, GBR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else {
        FastLED.addLeds<WS2812, WLED_PIN, BGR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);
    }

    FastLED.clear(true);
    FastLED.setBrightness(brightness);
    fill_solid(leds, appConfig.count, CRGB(r, g, b));
    FastLED.show();

    /*

    // first check if to blink
    if (blink) {
        FastLED.show();
        delay(blink_delay * 1000);
        FastLED.clear(true);
        delay(blink_delay * 1000);

        // turn off after delay
        if (turn_off && turn_off_delay > 0) {
            delay(turn_off_delay * 1000);
            FastLED.clear(true);
        }

    } else {
        FastLED.show();

        if (turn_off && turn_off_delay > 0) {
            delay(turn_off_delay * 1000);
            FastLED.clear(true);
        }
    }*/
}

// Color Control action. Perform action defined in mapping
void actionColor(String color, const char* r_pin, const char* g_pin, const char* b_pin, const char* ww_pin, const char* cw_pin, int brightness, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {
    uint8_t r, g, b;
    hexToRgb(color, r, g, b);
    int pinR = outputToPin(r_pin);
    int pinG = outputToPin(g_pin);
    int pinB = outputToPin(b_pin);
    int pinWW = outputToPin(ww_pin);
    int pinCW = outputToPin(cw_pin);

    // first check if to blink
    if (blink) {
        analogWrite(pinR, r);
        analogWrite(pinG, g);
        analogWrite(pinB, b);
        analogWrite(pinWW, 0);
        analogWrite(pinCW, 0);
        delay(blink_delay * 1000);

        analogWrite(pinR, 0);
        analogWrite(pinG, 0);
        analogWrite(pinB, 0);
        analogWrite(pinWW, 0);
        analogWrite(pinCW, 0);
        delay(blink_delay * 1000);

        // turn off after delay
        if (turn_off && turn_off_delay > 0) {
            delay(turn_off_delay * 1000);
            analogWrite(pinR, 0);
            analogWrite(pinG, 0);
            analogWrite(pinB, 0);
            analogWrite(pinWW, 0);
            analogWrite(pinCW, 0);
        }

    } else {
        analogWrite(pinR, r);
        analogWrite(pinG, g);
        analogWrite(pinB, b);
        analogWrite(pinWW, 0);
        analogWrite(pinCW, 0);

        if (turn_off && turn_off_delay > 0) {
            delay(turn_off_delay * 1000);
            analogWrite(pinR, 0);
            analogWrite(pinG, 0);
            analogWrite(pinB, 0);
            analogWrite(pinWW, 0);
            analogWrite(pinCW, 0);
        }
    }
}

// Maintenance action. Perform action defined in mapping
void actionMaintenance() {
    if (!maintenancenToggle) {
        if (appConfig.wled) {
            actionColorWled("#ffffff", 125);

        } else if (appConfig.analog) {
            if (appConfig.mode == 1) {
                actionColor("#ffffff", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 255);
            }
        }
        maintenancenToggle = true;
    } else {
        if (appConfig.wled) {
            FastLED.clear(true);

        } else if (appConfig.analog) {
            if (appConfig.mode == 1) {
                actionColor("#000000", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 0);
            }
        }

        maintenancenToggle = false;
    }
}