#pragma once
#include <Config.h>
#include <Defines.h>
#include <Shaders.h>

namespace LEDHandler
{
    LiteLED ledStrip(LED_TYPE, LED_TYPE_IS_RGBW);
    char* shaderName = strdup("solid");
    char* baseColorHex = strdup("#000000");
    bool power = Config::getPower();
    int baseColor = 0x000000;
    unsigned long lastRender = millis();
    Shaders::ShaderEffectFunc shader = Shaders::getShaderByName(shaderName);

    void init()
    {
        ledStrip.begin(LED_PIN, NUM_LEDS);
        ledStrip.brightness(200);
    }
    
    void checkShaderChange() {
        if(strcmp(Config::getEffect(), shaderName) != 0) {
            shaderName = strdup(Config::getEffect());
            shader = Shaders::getShaderByName(Config::getEffect());
        }
        if(strcmp(Config::getColor(), baseColorHex) != 0) {
            baseColorHex = strdup(Config::getColor());
            baseColor = hexFromString(String(baseColorHex));
        }
    }

    void loop()
    {
        if(power != Config::getPower()) {
            power = Config::getPower();
            if (!power) {
                ledStrip.clear(1);
            }
        }
        
        if(!power) {
            delay(250);
            return;
        }

        checkShaderChange();

        unsigned long currentTime = millis();
        unsigned long deltaTime = currentTime - lastRender;

        int ledIndex = 0;
        Shaders::shader_bundle_t bundle = {
            .baseColor = baseColor,
            .time = currentTime,
            .deltaTime = deltaTime,
        };

        for (; ledIndex < NUM_LEDS; ledIndex++) {
            uint32_t color = shader(bundle, ledIndex);
            int correctedColor = colorCorrect(color, CustomCorrection);
            ledStrip.setPixel(ledIndex, correctedColor, 0);
            //Serial.printf("%d rgb(%f, %f, %f)\n", ledIndex, hexToOkRGB(correctedColor).r, hexToOkRGB(correctedColor).g, hexToOkRGB(correctedColor).b);
        }

        ledStrip.brightness(Config::getBrightness());
        ledStrip.show();

        lastRender = millis();
        delay(1000 / FRAMES_PER_SECOND);
    }

}