#include <Arduino.h>
#include <Config.h>
#include <WiFiHandler.h>
#include <LEDHandler.h>
#include <WebServerHandler.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("Mycelium starting...");

  delay(500);
  Config::init();

  delay(500);
  LEDHandler::init();

  delay(500);
  WiFiHandler::init();

  delay(500);
  WebServerHandler::init();
}

void loop()
{
  WiFiHandler::loop();
  WebServerHandler::loop();
  LEDHandler::loop();
}