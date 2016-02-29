/*

Stand alone momentary push button to publish message to MQTT server

When CHPD is connected to 3.3v by a switch, ESP wil connect to wifi
and publish on topic. Openhab will watch this Topic and toggle the 
switch automatically.

VOIDS
- Setup
- Loop
- setup_wifi
- reconnect

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "SSIDPASS.h"


// GPIO Defines //
int holdPin = 0; // defines GPIO 0 as the hold pin (will hold CH_PD high untill we power down).
char const* outTopic = "/DEBUG/";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(holdPin, OUTPUT);     // sets GPIO 0 to output
  digitalWrite(holdPin, HIGH);  // sets GPIO 0 to high (this holds CH_PD high when the button is relesed).
 
  Serial.begin(9600);           // Start Serial Monitor
  Serial.println("Setup done");
  
  setup_wifi();                 // Setup WiFi
  client.setServer(server, 1883);
 
  reconnect();                  // Setup MQTT

}

void loop(){
      
  client.publish(Topic, "ON");

  digitalWrite(holdPin, LOW);    // set GPIO 0 low this takes CH_PD & powers down the ESP.
}


/*
------------------------------------------------------------------------------------
                         VOIDS
------------------------------------------------------------------------------------
*/
void setup_wifi() {

  delay(10);
  // Start by connecting to a WiFi network
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

  while (!client.connected()) {     // Loop until we're reconnected
    Serial.println("Attempting MQTT connection...");
    
    if (client.connect(clientid)) {  // ***NEEDS TO BE UNIQUE***
      Serial.println("MQTT Connected");
   
      // Once connected, publish an announcement...
      client.publish(outTopic, debugmess);  // To help debug when device is connected
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
