#include <Arduino.h>
#include <FastLED.h>
#include <Preferences.h>

Preferences pref;
bool maintenancenToggle = false;

/**
 * Helper Functions
 */

int outputToPin(String output) {
    if (output == "analog-r") {
        return 17;
    } else if (output == "analog-g") {
        return 16;
    } else if (output == "analog-b") {
        return 4;
    } else if (output == "analog-ww") {
        return 15;
    } else if (output == "analog-cw") {
        return 2;
    } else {
        // default
        return output.toInt();
    }
}

EOrder colorOrderHelper(String order) {
    if (order == "rgb") {
        return RGB;
    } else if (order == "rbg") {
        return RBG;
    } else if (order == "grb") {
        return GRB;
    } else if (order == "gbr") {
        return GBR;
    } else if (order == "brg") {
        return BRG;
    } else if (order == "bgr") {
        return BGR;
    } else {
        return GRB;
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

// Pin Control action. Perform action defined in mapping
void actionPinControl(String pin, String control, bool invert = false, int invert_delay = 0, int flash_timout = 0, int flash_count = 0) {
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
void actionColorWled(String color, int brightness, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {
    const int wledPin = 18;

    pref.begin("deviceSettings", true);
    const int wledPixel = pref.getInt("count", 10);
    const String order = pref.getString("order", "gbr");
    pref.end();

    // EOrder colorOrder = colorOrderHelper(order);
    CRGB leds[wledPixel];
    uint8_t r, g, b;
    hexToRgb(color, r, g, b);

    // setup fastled
    FastLED.clear(true);
    FastLED.addLeds<WS2812, wledPin, GBR>(leds, wledPixel).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);
    fill_solid(leds, wledPixel, CRGB(r, g, b));

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
    }
}

// Color Control action. Perform action defined in mapping
void actionColor(String color, String r_pin, String g_pin, String b_pin, String ww_pin, String cw_pin, int brightness, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {
    uint8_t r, g, b;
    hexToRgb(color, r, g, b);
    int pinR = outputToPin(r_pin);
    int pinG = outputToPin(g_pin);
    int pinB = outputToPin(b_pin);
    int pinWW = outputToPin(ww_pin);
    int pinCW = outputToPin(cw_pin);

    // setup analog leds
    pinMode(pinR, OUTPUT);
    pinMode(pinG, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinWW, OUTPUT);
    pinMode(pinCW, OUTPUT);

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
    pref.begin("deviceSettings", true);
    bool wled = pref.getBool("wled", true);
    bool analog = pref.getBool("analog", true);
    String mode = pref.getString("mode", "strip");
    pref.end();

    if (!maintenancenToggle) {
        if (wled) {
            actionColorWled("#ffffff", 255);

        } else if (analog) {
            if (mode == "strip") {
                actionColor("#ffffff", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 255);
            }
        }
        maintenancenToggle = true;
    } else {
        if (wled) {
            FastLED.clear(true);

        } else if (analog) {
            if (mode == "strip") {
                actionColor("#000000", "analog-r", "analog-g", "analog-b", "analog-ww", "analog-cw", 0);
            }
        }

        maintenancenToggle = false;
    }
}