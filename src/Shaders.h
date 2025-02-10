#pragma once
#include <map>
#include <string>
#include <FastLED.h>
#include <Defines.h>
#include <Config.h>

namespace Shaders
{
    struct shader_bundle_t {
        unsigned int*  index;
        CRGB*          baseColor;
        unsigned long* time;
        unsigned long* deltaTime;
    };

    typedef CRGB (*ShaderEffectFunc)(shader_bundle_t&);

    CRGB effect_solid(shader_bundle_t& bundle) {
        return *bundle.baseColor;
    }

    CRGB effect_breathe(shader_bundle_t& bundle) {
        CRGB color = CRGB(bundle.baseColor->as_uint32_t());
        unsigned long elapsedTime = *bundle.time % 10000;
        float brightness;

        if (elapsedTime < 5000) {
            brightness = 1.0 - (elapsedTime / 5000.0);
        } else {
            brightness = (elapsedTime - 5000) / 5000.0;
        }
        
        return color.fadeToBlackBy((1.0 - brightness) * 255);
    }

    CRGB effect_rainbow(shader_bundle_t& bundle) {
        CHSV hsv;
        hsv.hue = map(*bundle.index, 0, NUM_LEDS, 0, 360);
        hsv.val = 255;
        hsv.sat = 240;

        return CRGB(hsv);
    }
    
    CRGB effect_blink(shader_bundle_t& bundle) {
        if ((*bundle.time / 500) % 2 == 0) {
            return CRGB::Red;
        } else {
            return CRGB::Black;
        }
    }

    std::map<std::string, ShaderEffectFunc> shaderEffects = {
        {"solid", effect_solid},
        {"breathe", effect_breathe},
        {"rainbow", effect_rainbow},
        {"blink", effect_blink}
    };

    ShaderEffectFunc getShaderByName(char* name) {
        if (shaderEffects.find(name) != shaderEffects.end()) {
            return shaderEffects[name];
        }

        return shaderEffects["solid"];
    }
}