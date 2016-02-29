/*

Garage controller
Controls 2 relays, senses door open/closed, senses movement.

-*-Relay-*-
On startup relay state is published to ON (Light connected on relay NC terminals)
Listens on TopicCOMRELAY1 and TopicCOMRELAY2 for ON/OFF command then updates state.

_*-Door-*-
When door opens/closes sate is published to TopicDOOR

-*-PIR-*-
When movement is sensed "open" sate is published to TopicPIR, After set time (pause) set back to "closed"


VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect
- PIR
- DoorOpen

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// PIR DEFINES //

// Change Topic to the topic you want to control
char const* TopicPIR = "/room/pir/";

// the time we give the sensor to calibrate (10-60 secs according to the datasheet)
// int calibrationTime = 30;        
int calibrationTime = 10;        

// the time when the sensor outputs a low impulse
long unsigned int lowIn;         

// the amount of milliseconds the sensor has to be low 
// before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 4;    // the digital pin connected to the PIR sensor's output

// DOOR DEFINES //

// Change Topic to the topic you want to control
const char* TopicDOOR = "/room/door/";

int buttonPin = 5;
boolean currentState = LOW;// stroage for current button state
boolean lastState = LOW;// storage for last button state
boolean doorState = LOW;// storage for the current state of the LED (off/on)


// PIN Defines //
int RELAY1 = 12;
int RELAY2 = 13;

// WiFi DEFINES //

char const* ssid = "..........";           // Router SSID
char const* password = "..........";       // Router Passcode
char const* server = "..........";         // Mosquitto Server IP

// Topic Defines //
char const* TopicCOMRELAY1 = "/gar/relay/1/com/";
char const* TopicCOMRELAY2 = "/gar/relay/2/com/";
char const* TopicSTATERELAY1 = "/gar/relay/1/state/";
char const* TopicSTATERELAY2 = "/gar/relay/2/state/";

char const* outTopic = "/DEBUG/";


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(RELAY1, OUTPUT);     // Initialize the RELAY pin as an output
  pinMode(RELAY2, OUTPUT);     // Initialize the RELAY pin as an output
  Serial.begin(9600);

  delay(2000);
  Serial.println("Setup done");
  
  setup_wifi();
  client.setServer(server, 1883);
  client.setCallback(callback);

  // PIR SETUP
//give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);

    // DOOR SETUP
pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
    pinMode(buttonPin, OUTPUT);
  digitalWrite(buttonPin, LOW);

  // Publish Relay1 ON start up
client.publish(TopicSTATERELAY1,"ON");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  PIR();
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
 
    if (client.connect("RelayDoorPirOLED")) {     // ***NEEDS TO BE UNIQUE***
      Serial.println("MQTT Connected");
      Serial.println("MQTT Connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "ESP RelayDoorPirOLED Connected");
      // ... and resubscribe
      client.subscribe(TopicCOMRELAY1);
      client.subscribe(TopicCOMRELAY2);

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

void PIR()
{
   if (client.connect("arduinoClient"))
  {
     if(digitalRead(pirPin) == HIGH){
       if(lockLow){  
         // makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         client.publish(TopicPIR,"OPEN");
   
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       

       if(takeLowTime){
        lowIn = millis();          // save the time of the transition from high to LOW
        takeLowTime = false;       // make sure this is only done at the start of a LOW phase
        }
       // if the sensor is low for more than the given pause, 
       // we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           // makes sure this block of code is only executed again after 
           // a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
           client.publish(TopicPIR,"CLOSED");
           delay(50);
           }
       }
     }
  }

void DoorOpen()
{
  //client.loop();
  currentState = digitalRead(buttonPin);

if (currentState != lastState){
  
  if (currentState == HIGH){
      delay(100);//crude form of button debouncing
      Serial.println("Closed");
      client.publish(TopicDOOR,"CLOSED");
    }
    
    else
    {
      delay(100);//crude form of button debouncing
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

  // Switch on the RELAY if an 1 was received as first character
    if (strcmp(topic,TopicCOMRELAY2)==0) { 
  if ((char)payload[0] == '1') {
    digitalWrite(RELAY2, HIGH); 
    client.publish(TopicSTATERELAY2,"ON");
    
  } else {
    digitalWrite(RELAY2, LOW);
    client.publish(TopicSTATERELAY2,"OFF"); 
  }

}
}
