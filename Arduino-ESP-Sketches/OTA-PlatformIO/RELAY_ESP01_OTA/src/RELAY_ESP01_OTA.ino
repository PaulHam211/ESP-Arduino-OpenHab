/*

Basic 1 light/relay control

-*-Relay-*-
On startup relay state is published to ON (Light connected on relay NC terminals)
Listens on TopicCOMRELAY1 for ON/OFF command then updates state.

VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "SSIDPASS.h"
#include "OTA.h"

char const* outTopic = "/DEBUG/";

// PIN Defines //
int RELAY1 = 0;


WiFiClient espClient;
PubSubClient client(espClient);


void setup() {

  OTA_Setup();

  pinMode(RELAY1, OUTPUT);     // Initialize the LED pin as an output

  Serial.println("Setup done");

//setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);
reconnect();
  client.publish(TopicSTATERELAY1,"ON");
}

void loop() {
  OTA_Loop();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  }


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect(clientid)) {      // ***NEEDS TO BE UNIQUE***
      Serial.println("MQTT Connected");
      Serial.println("MQTT Connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, debugmess);

      // ... and resubscribe
      client.subscribe(TopicCOMRELAY1);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  //Serial.print(inTopic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();

  // Switch on the RELAY if an 1 was received as first character
    if (strcmp(topic,TopicCOMRELAY1)==0) {
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY1, LOW);
    client.publish(TopicSTATERELAY1,"ON");

  } else {
    digitalWrite(RELAY1, HIGH);
    client.publish(TopicSTATERELAY1,"OFF");
  }
 }
}
