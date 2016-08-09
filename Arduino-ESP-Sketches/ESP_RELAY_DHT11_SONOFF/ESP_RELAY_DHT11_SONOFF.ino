/*

DHT Sensor
Measures Temp & Humidity & publishes message to topic


VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect
- DHTSensor

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "Config.h"
#include "SSIDPASS.h"

// DHT DEFINES //
#define DHTPIN 14     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

// PIN Defines //
int RELAY1 = 12;
int LED = 13;

// WiFi DEFINES //
// SSID & Passcode set in SSIDPASS.h


char const* outTopic = "/DEBUG/";

WiFiClient espClient;
PubSubClient client(espClient);


unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change :
const long interval = 1800000;           // 0.5hr interval at which to send Temp (milliseconds)

void setup() {
  pinMode(RELAY1, OUTPUT);     // Initialize the RELAY pin as an output
  pinMode(LED, OUTPUT);     // Initialize the LED pin as an output
  digitalWrite(LED, LOW);   // Turn the LED ON
  Serial.begin(9600);
  
setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);
  dht.begin();

  // Publish Relay1 OFF start up
client.publish(TopicSTATERELAY1,"OFF");
}

void loop() {

  if (!client.connected()) {
     reconnect();
  }
   client.loop();
  DHTSensor();
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

    if (client.connect(clientid)) {
      Serial.println("MQTT Connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, debugmess);
      // ... and resubscribe
      client.subscribe(TopicCOMRELAY1);
  digitalWrite(LED, LOW);   // Turn the LED ON
  delay(500);               // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED OFF
  delay(500);               // Wait for a second
  digitalWrite(LED, LOW);   // Turn the LED ON
  delay(500);               // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED OFF

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      // Wait 5 seconds before retrying
      delay(5000);
    }
   if(digitalRead(RELAY1) == LOW){
      client.publish(TopicSTATERELAY1,"OFF");
      }
      }
  }


void DHTSensor()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();


  // Check if any reads failed and exit early (to try again).
   if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    client.publish(outTopic, "Failed to read from DHT sensor!" );  
    return;
  }
      // Compute heat index in Celsius (isFahreheit = false)
      char t2[50];
      char h2[50];
      dtostrf(t, 5, 2, t2);
      dtostrf(h, 5, 2, h2);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
      
     client.publish(TopicTEMP, t2 );  
     client.publish(TopicHUM, h2 );
  digitalWrite(LED, LOW);   // Turn the LED ON
  delay(250);               // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED OFF
      }
      }
      

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(TopicCOMRELAY1);
  Serial.print("] - ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
    
  // Switch on the RELAY if an 1 was received as first character
    if (strcmp(topic,TopicCOMRELAY1)==0) { 
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY1, HIGH);
    client.publish(TopicSTATERELAY1,"ON");
    
  } else {
    digitalWrite(RELAY1, LOW);
    client.publish(TopicSTATERELAY1,"OFF"); 
  }

    }
    
  digitalWrite(LED, LOW);   // Turn the LED ON
  delay(250);               // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED OFF
  delay(250);               // Wait for a second
  digitalWrite(LED, LOW);   // Turn the LED ON
  delay(250);               // Wait for a second
  digitalWrite(LED, HIGH);  // Turn the LED OFF
}
  

