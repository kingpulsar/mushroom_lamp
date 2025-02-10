#pragma once
#include <Arduino.h>
#include <FastLED.h>

bool startsWith(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

CRGB hexToCRGB(const String &hexString) {
    if (hexString[0] != '#' || hexString.length() != 7) {
        return CRGB::Black;
    }

    uint32_t hexValue = strtoul(hexString.substring(1).c_str(), NULL, 16);

    uint8_t red   = (hexValue >> 16) & 0xFF;
    uint8_t green = (hexValue >> 8)  & 0xFF;
    uint8_t blue  = hexValue         & 0xFF;

    return CRGB(red, green, blue);
}