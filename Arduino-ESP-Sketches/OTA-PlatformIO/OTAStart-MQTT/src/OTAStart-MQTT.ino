#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>

#include "SSIDPASS.h"
#include "Config.h"
#include "OTA.h"

unsigned long oldtime = 0;
unsigned long basetime;
unsigned long timetaken;


char const* outTopic = "/DEBUG/";

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
  OTA_Setup();
  client.setServer(server, 1883);
}

void loop() {
  OTA_Loop();

  if (!client.connected()) {

unsigned long basetime = millis();
timetaken = basetime - oldtime;
timetaken = timetaken/1000;
oldtime = basetime;

    Serial.print("Disconnected after ");
    Serial.print(timetaken);
    Serial.println(" seconds.");

     reconnect();
  }
  delay(100);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
      // Attempt to connect

      String clientId = "ESP8266Client-";
      clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT Connected");

      // Once connected, publish an announcement...
      //client.publish(outTopic, debugmess);
      String ipaddress = debugmess + WiFi.localIP().toString();
      char ipchar[ipaddress.length()+1];
      ipaddress.toCharArray(ipchar,ipaddress.length()+1);
      client.publish(outTopic, ipchar);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
