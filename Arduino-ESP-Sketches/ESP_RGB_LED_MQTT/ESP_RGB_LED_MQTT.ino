/*

RGB LED Strip controller

On startup WiFi state green is displayed through LED strip once connected to Wifi network and to MQTT server.
Listens on /openHAB/nl/RED, /openHAB/nl/GREEN, /openHAB/nl/BLUE for colour information command then updates LED.

VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi DEFINES //
const char* ssid = "..........";           // Router SSID
const char* password = "..........";       // Router Passcode
char* outTopic = "/DEBUG/";                // Debug topic

// Mosquitto server IP address
  IPAddress server(192, 168, 1, 10);       // Mosquitto Server IP

  WiFiClient wclient;
  PubSubClient client(wclient);

// Timers
  int timer = 0;
  int EndTimer = 200; 

// RGB LED pins
  int LEDred=5;   // D1 - GPIO5
  int LEDgreen=4; // D2 - GPIO4
  int LEDblue=2;  // D4 - GPIO2
 
// Transmitted colour variables
  int LEDredValue = 0;
  int LEDgreenValue = 0;
  int LEDblueValue = 0;

// Startup variables
  int CURredValue = 0;
  int CURgreenValue = 0;
  int CURblueValue = 0;

void setup() {
  
// Set callback  
  client.setServer(server, 1883);
  client.setCallback(callback);
  
// Set pins as outputs & While connecting to WIFI set Red
  pinMode(LEDblue, OUTPUT);
  pinMode(LEDred, OUTPUT);
  pinMode(LEDgreen, OUTPUT);
 
// Start serial monitor
  Serial.begin(9600);           // Start Serial Monitor
 setup_wifi();
if (WiFi.waitForConnectResult() != WL_CONNECTED)
      return;
    Serial.println("WiFi connected");
    
// Set green on sucessful WIFI connection
    LEDgreenValue = 100;
 
// Set startup colour as red
  analogWrite(LEDred, 100);
  analogWrite(LEDgreen, 0);
  analogWrite(LEDblue, 0);
    }

void loop() {

  if (!client.connected()) {
    reconnect();
  }

 if (timer <= EndTimer) timer++;
 else {
 timer = 0;
 
 if (LEDredValue < CURredValue) CURredValue--;
 else if (LEDredValue > CURredValue) CURredValue++;
 
 if (LEDgreenValue < CURgreenValue) CURgreenValue--;
 else if (LEDgreenValue > CURgreenValue) CURgreenValue++;
 
 if (LEDblueValue < CURblueValue) CURblueValue--;
 else if (LEDblueValue > CURblueValue) CURblueValue++;
    }
 
  analogWrite(LEDred, CURredValue);
  analogWrite(LEDgreen, CURgreenValue);
  analogWrite(LEDblue, CURblueValue);

  client.loop();
    }
    

   
// ===========================================================
// Callback function of MQTT. The function is called
// If a value has been received.
// ===========================================================
void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
int i = 0;
  Serial.println((char)payload[i]);
 
char message_buff[100];
 
for(i=0; i<length; i++) {
 message_buff[i] = payload[i];
 }
 message_buff[i] = '\0';
 
String msgString = String(message_buff);
 
 if (String(topic) == "/openHAB/nl/RED") LEDredValue = round(msgString.toInt() * 2.55);
 if (String(topic) == "/openHAB/nl/GREEN") LEDgreenValue = round(msgString.toInt() * 2.55);
 if (String(topic) == "/openHAB/nl/BLUE") LEDblueValue = round(msgString.toInt() * 2.55);
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

    if (client.connect("RGBConservatory")) {    // ***NEEDS TO BE UNIQUE***
  client.publish(outTopic, "RGB Mood Connected");
  client.subscribe("/openHAB/nl/RED/#");
  client.subscribe("/openHAB/nl/GREEN/#");
  client.subscribe("/openHAB/nl/BLUE/#");
  Serial.println("Subscribed to topics");
  Serial.println("Waiting for MQTT Commands");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

