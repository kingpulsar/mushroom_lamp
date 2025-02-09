#pragma once
#include <Config.h>
#include <FastLED.h>

namespace LEDHandler
{
#define LED_PIN GPIO_NUM_4
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define NUM_LEDS 5

#define BRIGHTNESS 255
#define FRAMES_PER_SECOND 20

    CRGB leds[NUM_LEDS];

    void init()
    {
        FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(BRIGHTNESS);
    }

    void loop()
    {
        long color = strtol(Config::getColor(), NULL, 0);
        fill_solid(leds, NUM_LEDS, color);

        FastLED.show();
        FastLED.delay(1000 / FRAMES_PER_SECOND);
    }

}