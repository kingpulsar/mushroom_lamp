#pragma once
#include <map>
#include <string>
#include <Defines.h>
#include <Config.h>
#include <easing.h>
#include <ok_color.h>

namespace Shaders
{
    easingFunction ease = getEasingFunction(EaseInOutCubic);

    struct shader_bundle_t {
        int      baseColor;
        unsigned long time;
        unsigned long deltaTime;
    };

    typedef int (*ShaderEffectFunc)(shader_bundle_t&, int ledIndex);

    int effect_solid(shader_bundle_t& bundle, int ledIndex) {
        return bundle.baseColor;
    }

    int effect_breathe(shader_bundle_t& bundle, int ledIndex) {
        int color = bundle.baseColor;
        
        ok_color::RGB baseRGB = hexToOkRGB(color);
        ok_color::HSL baseHSL = ok_color::srgb_to_okhsl(baseRGB);

        unsigned long elapsedTime = bundle.time % 10000;
        float brightness;

        if (elapsedTime < 5000) {
            brightness = 1.0 - (elapsedTime / 5000.0);
        } else {
            brightness = (elapsedTime - 5000) / 5000.0;
        }

        ok_color::HSL hsl = {
            .h = fmod(baseHSL.h + (brightness / 180.0f), 1.0f),
            .s = baseHSL.s,
            .l = (float)brightness,
        };
        
        return okRGBToHex(ok_color::okhsl_to_srgb(hsl));
    }

    int effect_rainbow(shader_bundle_t& bundle, int ledIndex) {
        float degrees = fmod(((ledIndex * 6) + (bundle.time / 150)), 360.0f);
        float fract = mapfloat(degrees, 0.0f, 360.0f, 0.0f, 1.0f);

        ok_color::HSL hsl = {
            .h = fract,
            .s = 0.85f,
            .l = 0.5f,
        };

        return okRGBToHex(ok_color::okhsl_to_srgb(hsl));
    }
    
    int effect_blink(shader_bundle_t& bundle, int ledIndex) {
        if ((bundle.time / 500) % 2 == 0) {
            return bundle.baseColor;
        } else {
            return 0x000000;
        }
    }

    int effect_gradient(shader_bundle_t& bundle, int ledIndex) {
        float fract = (float)ledIndex / NUM_LEDS;

        ok_color::RGB color1 = hexToOkRGB(bundle.baseColor);
        ok_color::RGB color2 = { 0.5f, 0.0f, 1.0f };
        ok_color::RGB color3 = ok_color::lerpOkLab(color1, color2, fract);

        Serial.printf("Index: %d, Fract: %f\n", ledIndex, fract);
        Serial.printf("c1: %f, %f, %f\n", color1.r, color1.g, color1.b);
        Serial.printf("c2: %f, %f, %f\n", color2.r, color2.g, color2.b);
        Serial.printf("c3: %f, %f, %f\n\n", color3.r, color3.g, color3.b);

        return okRGBToHex(color3);
    }

    std::map<std::string, ShaderEffectFunc> shaderEffects = {
        {"solid", effect_solid},
        {"breathe", effect_breathe},
        {"rainbow", effect_rainbow},
        {"blink", effect_blink},
        {"gradient", effect_gradient}
    };

    ShaderEffectFunc getShaderByName(char* name) {
        if (shaderEffects.find(name) != shaderEffects.end()) {
            return shaderEffects[name];
        }

        return shaderEffects["solid"];
    }
}