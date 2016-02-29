/*

Relay and Door module
Controls 2 relays and senses door open/closed.

-*-Relay-*-
On startup relay state is published to ON (Light connected on relay NC terminals)
Listens on TopicCOMRELAY1 for ON/OFF command then updates state.

_*-Door-*-
When door opens/closes sate is published to TopicDOOR


VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect
- DoorOpen

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "SSIDPASS.h"

// DOOR DEFINES //
int buttonPin = 2;
boolean currentState = LOW;   //Storage for current button state
boolean lastState = LOW;      //Storage for last button state
boolean doorState = LOW;      //Storage for the current state of the Door

char const* outTopic = "/DEBUG/";           // Debug topic

// PIN Defines //
int RELAY1 = 0;


  WiFiClient espClient;
  PubSubClient client(espClient);


void setup() {
  pinMode(RELAY1, OUTPUT);       // Initialize the Relay pin as an output
  Serial.begin(9600);           // Start Serial Monitor
  Serial.println("Setup done");
  
  setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);

// DOOR SETUP
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
  pinMode(buttonPin, OUTPUT);
  digitalWrite(buttonPin, LOW);

    // Publish Relay1 to ON start up
  client.publish(TopicSTATERELAY1,"ON");
  }

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  DoorOpen();
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

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      // Wait 5 seconds before retrying
      delay(5000);
    }
    if(digitalRead(RELAY1) == LOW){
      client.publish(TopicSTATERELAY1,"ON");
      }
  }
}

void DoorOpen()
{
  currentState = digitalRead(buttonPin);

if (currentState != lastState){
  
  if (currentState == HIGH){
      delay(100);//crude form of button debouncing
      Serial.println("Closed");
      client.publish(TopicDOOR,"CLOSED");
    }
    
    else
    {
      delay(100);       //crude form of button debouncing
      Serial.println("Open");
      doorState = HIGH;
      client.publish(TopicDOOR,"OPEN");
    } 
    }
lastState = currentState;
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
