#pragma once
#include <Config.h>
#include <FastLED.h>
#include <Defines.h>
#include <Shaders.h>

namespace LEDHandler
{
    CRGB leds[NUM_LEDS];
    char* shaderName = strdup("solid");
    char* baseColorHex = strdup("#000000");
    bool power = Config::getPower();
    CRGB baseColor = CRGB::Black;
    unsigned long lastRender = millis();
    Shaders::ShaderEffectFunc shader = Shaders::getShaderByName(shaderName);

    void init()
    {
        FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    }
    
    void checkShaderChange() {
        if(strcmp(Config::getEffect(), shaderName) != 0) {
            shaderName = strdup(Config::getEffect());
            shader = Shaders::getShaderByName(Config::getEffect());
        }
        if(strcmp(Config::getColor(), baseColorHex) != 0) {
            baseColorHex = strdup(Config::getColor());
            baseColor = hexToCRGB(baseColorHex);
        }
    }

    void loop()
    {

        if(power != Config::getPower()) {
            power = Config::getPower();
            if (!power) {
                fill_solid(leds, NUM_LEDS, CRGB::Black);
                FastLED.show();
            }
        }
        
        if(!power) {
            delay(250);
            return;
        }

        checkShaderChange();
        
        FastLED.setBrightness(Config::getBrightness());
        unsigned long currentTime = millis();
        unsigned long deltaTime = currentTime - lastRender;

        unsigned int ledIndex = 0;
        Shaders::shader_bundle_t bundle = {
            .index = &ledIndex,
            .baseColor = &baseColor,
            .time = &currentTime,
            .deltaTime = &deltaTime,
        };

        for (; ledIndex < NUM_LEDS; ledIndex++) {
            leds[ledIndex] = shader(bundle);
            //Serial.printf("%d rgb(%d, %d, %d)\n", ledIndex, leds[ledIndex].r, leds[ledIndex].g, leds[ledIndex].b);
        }
        FastLED.show();

        lastRender = millis();
        FastLED.delay(1000 / FRAMES_PER_SECOND);
    }

}