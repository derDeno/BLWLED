
void setupWled() {
    leds = new CRGB[appConfig.count];

    if (String(appConfig.order).equalsIgnoreCase("rgb")) {
        FastLED.addLeds<WS2812B, WLED_PIN, RGB>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("rbg")) {
        FastLED.addLeds<WS2812B, WLED_PIN, RBG>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("brg")) {
        FastLED.addLeds<WS2812B, WLED_PIN, BRG>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("bgr")) {
        FastLED.addLeds<WS2812B, WLED_PIN, BGR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("grb")) {
        FastLED.addLeds<WS2812B, WLED_PIN, GRB>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else if (String(appConfig.order).equalsIgnoreCase("gbr")) {
        FastLED.addLeds<WS2812B, WLED_PIN, GBR>(leds, appConfig.count).setCorrection(TypicalLEDStrip);

    } else {
        FastLED.addLeds<WS2812B, WLED_PIN, GRB>(leds, appConfig.count).setCorrection(TypicalLEDStrip);
    }
}

void clearWled() {

}

void startupAnimation() {
    logger("Startup Animation");

    // wled rainbow animation if active
    if (appConfig.wled && appConfig.count > 0) {

        FastLED.clear(true);
        FastLED.setBrightness(255);

        int hue = 0;
        for (int i = 0; i < 500; i++) {
            fill_rainbow(leds, appConfig.count, hue, 10);
            FastLED.show();
            hue++;
            delay(10);
        }

        FastLED.clear(true);
    }

    // analog animation if active
    if (appConfig.analog) {
        const int ANALOG_DELAY_MS = 250;

        analogWrite(ANALOG_PIN_R, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_R, 0);

        analogWrite(ANALOG_PIN_G, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_G, 0);

        analogWrite(ANALOG_PIN_B, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_B, 0);

        analogWrite(ANALOG_PIN_B, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_B, 0);

        analogWrite(ANALOG_PIN_WW, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_WW, 0);

        analogWrite(ANALOG_PIN_CW, 255);
        delay(ANALOG_DELAY_MS);
        analogWrite(ANALOG_PIN_CW, 0);
    }

}