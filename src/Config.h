#pragma once
#include <Arduino.h>
#include <utils.h>
#include <Preferences.h>
#include <string.h>

namespace Config {

    const char* configKey = "mushroom_config";
    Preferences prefs;

    bool PREF_POWER = false;
    char* PREF_COLOR = strdup("#FF0000");
    uint8_t PREF_BRIGHTNESS = 255;
    char* PREF_EFFECT = strdup("solid");

    char* toString() {
        static char output[100];
        snprintf(output, sizeof(output), "config|%s|%s|%s|%s", PREF_POWER ? "on" : "off", PREF_COLOR, String(PREF_BRIGHTNESS), PREF_EFFECT);
        return output;
    }

    void debugConfig() {
        Serial.println(Config::toString());
    }

    void writeConfigToEeprom() {
        prefs.putBool("power",  PREF_POWER);
        prefs.putString("color", PREF_COLOR);
        prefs.putUChar("brightness", PREF_BRIGHTNESS);
        prefs.putString("effect", PREF_EFFECT);
    }

    void loadConfigFromEeprom() {
        if (prefs.isKey("power"))
            PREF_POWER = prefs.getBool("power", false);

        if (prefs.isKey("color"))
            PREF_COLOR = strdup(prefs.getString("color").c_str());

        if (prefs.isKey("brightness"))
            PREF_BRIGHTNESS = prefs.getUChar("brightness");

        if (prefs.isKey("effect"))
            PREF_EFFECT = strdup(prefs.getString("effect").c_str());
    }

    void init() {
        if (!prefs.begin(configKey, false)) {
            Serial.println("Failed to begin preferences");
        }

        loadConfigFromEeprom();
        debugConfig();
    }

    void setPower(bool power) {
        Serial.printf("Set power: %d\n", power);
        PREF_POWER = power;
        writeConfigToEeprom();
    }

    void setColor(char* color) {
        Serial.printf("Set color: %s\n", color);
        PREF_COLOR = color;
        writeConfigToEeprom();
    }

    void setBrightness(uint8_t brightness) {
        Serial.printf("Set brightness: %d\n", brightness);
        PREF_BRIGHTNESS = brightness;
        writeConfigToEeprom();
    }

    void setEffect(char* effect) {
        Serial.printf("Set effect: %s\n", effect);
        PREF_EFFECT = effect;
        writeConfigToEeprom();
    }

    boolean getPower() {
        return PREF_POWER;
    }

    char* getColor() {
        return PREF_COLOR;
    }

    const uint8_t getBrightness() {
        return PREF_BRIGHTNESS;
    }

    char* getEffect() {
        return PREF_EFFECT;
    }

    void fromString(char* input) {
        static char internalArray[5][50];
        static char* outputArray[5];
        int index = 0;
        char *token = strtok(input, "|");
        
        while (token != nullptr && index < 5) {
            strncpy(internalArray[index], token, 49);
            internalArray[index][49] = '\0';
            outputArray[index] = internalArray[index];
            index++;
            token = strtok(nullptr, "|");
        }

        setPower(startsWith(outputArray[1], "on"));
        setColor(outputArray[2]);
        setBrightness(String(outputArray[3]).toInt());
        setEffect(outputArray[4]);

        Serial.print("New config: ");
        debugConfig();
    }

}