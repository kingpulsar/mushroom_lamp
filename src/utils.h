#pragma once
#include <Arduino.h>
#include <ok_color.h>

//Taken from FastLED library
typedef enum {
    TypicalSMD5050=0xFFB0F0 /* 255, 176, 240 */,
    TypicalLEDStrip=0xFFB0F0 /* 255, 176, 240 */,
    Typical8mmPixel=0xFFE08C /* 255, 224, 140 */,
    TypicalPixelString=0xFFE08C /* 255, 224, 140 */,
    CustomCorrection=0xFFB0C5,
    UncorrectedColor=0xFFFFFF /* 255, 255, 255 */
} LEDColorCorrection;

bool startsWith(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

int hexFromString(const String &hexString) {
    if (hexString[0] != '#' || hexString.length() != 7) return 0x0;
    return strtoimax(hexString.substring(1).c_str(), NULL, 16);
}

int subtractColors(int color1, int color2)
{
    int red = max(0, (color1 >> 16) - (color2 >> 16));
    int green = max(0, ((color1 >> 8) & 0xFF) - ((color2 >> 8) & 0xFF));
    int blue = max(0, (color1 & 0xFF) - (color2 & 0xFF));

    return (red << 16) + (green << 8) + blue;
}

int colorCorrect(int colorHex, LEDColorCorrection correction) {
    int correctionRed   = (correction >> 16) & 0xFF;
    int correctionGreen = (correction >> 8)  & 0xFF;
    int correctionBlue  = correction         & 0xFF;

    int colorRed   = (colorHex >> 16) & 0xFF;
    int colorGreen = (colorHex >> 8)  & 0xFF;
    int colorBlue  = colorHex         & 0xFF;

    return (map(colorRed, 0, 255, 0, correctionRed) << 16) + (map(colorGreen, 0, 255, 0, correctionGreen) << 8) + map(colorBlue, 0, 255, 0, correctionBlue);
}

int rgbToHex(int red, int green, int blue) {
    return (red << 16) + (green << 8) + blue;
}

int okRGBToHex(ok_color::RGB rgb) {
    return rgbToHex(int(trunc(rgb.r * 255.0f)), int(trunc(rgb.g * 255.0f)), int(trunc(rgb.b * 255.0f)));
}


float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

float lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
}

ok_color::RGB hexToOkRGB(int colorHex) {
    ok_color::RGB color;

    color.r = ((colorHex >> 16) & 0xFF) / 255.0f;
    color.g = ((colorHex >> 8)  & 0xFF) / 255.0f;
    color.b = (colorHex         & 0xFF) / 255.0f;

    return color;
}