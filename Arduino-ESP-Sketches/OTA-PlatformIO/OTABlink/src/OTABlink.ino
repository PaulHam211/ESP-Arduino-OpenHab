#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "SSIDPASS.h"
#include "OTA.h"

const int ledPin = 4;       // the pin that the LED is attached to

void setup() {
  OTA_Setup();

  pinMode(ledPin, OUTPUT);
}

void loop() {
  OTA_Loop();
  digitalWrite(ledPin, HIGH); // turn the LED on (HIGH is the voltage level)
  delay(1000); // wait for 2 second
  digitalWrite(ledPin, LOW); // turn the LED off by making the voltage LOW
  delay(1000);
}
