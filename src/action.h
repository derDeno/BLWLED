#include <Arduino.h>

extern AppConfig appConfig;
extern CRGB *leds;



/**
 * Convert pin name to pin number
 * @param output    pin name
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


/**
 * Pin Control action. Perform action defined in mapping
 * @param pin               pin name
 * @param control           control action (on, off, flash)
 * @param invert            invert control
 * @param invert_delay      delay for invert in seconds
 * @param flash_timout      flash timeout in seconds
 * @param flash_count       flash count
 */
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


/**
 * WLED Control action. Perform action defined in mapping
 * @param color             hex color value
 * @param brightness        brightness value 1-255
 * @param blink             blink yes/no
 * @param blink_delay       blink delay in seconds
 * @param turn_off          turn off after delay
 * @param turn_off_delay    turn off delay in seconds
 */
void actionColorWled(const char* color, int brightness = 255, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {

    const char* hexColor = color + 1;
    unsigned long colorValue = strtoul(hexColor, NULL, 16);

    FastLED.clear(true);
    FastLED.setBrightness(brightness);

    for (int i = 0; i < appConfig.count; i++) {
        leds[i] = CRGB(colorValue);
    }
    
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


// Turn off WLED
void actionColorWledOff() {
    FastLED.clear(true);
}


/**
 * Color Control action. Perform action defined in mapping
 * @param color             hex color value
 * @param r_pin             red pin
 * @param g_pin             green pin
 * @param b_pin             blue pin
 * @param ww_pin            warm white pin
 * @param cw_pin            cold white pin
 * @param brightness        brightness value 1-255
 * @param blink             blink yes/no
 * @param blink_delay       blink delay in seconds
 * @param turn_off          turn off after delay
 * @param turn_off_delay    turn off delay in seconds
 */
void actionColor(const char* color, const char* r_pin, const char* g_pin, const char* b_pin, const char* ww_pin, const char* cw_pin, int brightness, bool blink = false, int blink_delay = 0, bool turn_off = false, int turn_off_delay = 0) {

    const char* hexColor = color + 1;
    unsigned long colorValue = strtoul(hexColor, NULL, 16);
    uint8_t r = (colorValue >> 16) & 0xFF;
    uint8_t g = (colorValue >> 8) & 0xFF;
    uint8_t b = colorValue & 0xFF;

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


/**
 * Turn off color
 * @param pin       pin name or "all" for all pins
 */
void actionColorOff(const char* pin) {

    if(strcmp(pin, "all") == 0) {
        analogWrite(ANALOG_PIN_R, 0);
        analogWrite(ANALOG_PIN_G, 0);
        analogWrite(ANALOG_PIN_B, 0);
        analogWrite(ANALOG_PIN_WW, 0);
        analogWrite(ANALOG_PIN_CW, 0);
        return;
    }

    int outputPin = outputToPin(pin);
    analogWrite(outputPin, 0);
}

/**
 * WLED Rainbow action
 * @param brightness        brightness value 1-255
 * @param speed             how many hue steps to take 1-255
 * @param loop              loop animation
 */
void actionWledRainbow(int brightness = 255, int speed = 10, bool loop = false) {
    FastLED.setBrightness(brightness);

    int hue = 0;
    while (true) {
        fill_rainbow(leds, appConfig.count, hue, speed);
        FastLED.show();

         hue += speed;
        if (hue >= 256) {
            hue -= 256;
        }
        delay(10);
        yield();

        if (!loop && hue == 0) {
            break;
        }
    }
}