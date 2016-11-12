#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "SSIDPASS.h"
#include "OTA.h"


void setup() {
  OTA_Setup();
}

void loop() {
  OTA_Loop();
}
