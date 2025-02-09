#pragma once
#include "WiFi.h"
#include <esp_wifi.h>
#include <ESPmDNS.h>
#include <secrets.h>

namespace WiFiHandler
{
  
  static const char *hostname = "mushroom";

  void init()
  {

    WiFi.disconnect(true);
    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.println("Connecting to WiFi ...");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    }

    Serial.print("\nMushroom IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Mushroom hostname: ");
    Serial.println(WiFi.getHostname());
  }

  void loop()
  {
  }

}