/*

PIR Sensor
Senses movement.

-*-PIR-*-
When movement is sensed "open" sate is published to TopicPIR, After set time (pause) set back to "closed"


VOIDS
- Setup
- Loop
- callback
- setup_wifi
- reconnect
- PIR

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// PIR DEFINES //

// Change Topic to the topic you want to control
const char* TopicPIR = "/PIR/2/";

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

int pirPin = 2;    // the digital pin connected to the PIR sensor's output

// WiFi DEFINES //

char* ssid = "..........";           // Router SSID
char* password = "..........";       // Router Passcode
char* server = "..........";         // Mosquitto Server IP

char* outTopic = "/DEBUG/";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  Serial.begin(9600);
  
 void setup_wifi();
  client.setServer(server, 1883);
 // client.setCallback(callback);

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
}

void loop() {

  if (!client.connected()) {
     reconnect();
  }
   client.loop();
  PIR();
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

    if (client.connect("PIRSensor2")) {
      Serial.println("MQTT Connected");
      Serial.println("MQTT Connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "ESP PIRSensor2 Connected");
      // ... and resubscribe

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
     
      // Wait 5 seconds before retrying
      delay(5000);
    }
      }
  }


void PIR()
{
   if (client.connect("PIRSensor2"))
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

