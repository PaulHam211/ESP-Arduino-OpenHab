#include <PubSubClient.h>
#include <ESP8266WiFi.h>


// PIR DEFINES //

// Change Topic to the topic you want to control
const char* TopicPIR = "/room/pir/";

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
//int calibrationTime = 30;        
int calibrationTime = 10;        

//the time when the sensor outputs a low impulse
long unsigned int lowIn;         

//the amount of milliseconds the sensor has to be low 
//before we assume all motion has stopped
long unsigned int pause = 5000;  

boolean lockLow = true;
boolean takeLowTime;  

int pirPin = 4;    //the digital pin connected to the PIR sensor's output


// DOOR DEFINES //

// Change Topic to the topic you want to control
const char* TopicDOOR = "/room/door/";

int buttonPin = 5;
boolean currentState = LOW;//stroage for current button state
boolean lastState = LOW;//storage for last button state
boolean doorState = LOW;//storage for the current state of the LED (off/on)


// WiFi DEFINES //
char* ssid = "..........";           // Router SSID
char* password = "..........";       // Router Passcode


char* server = "192.168.1.10";

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);



void setup() {
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  digitalWrite(pirPin, LOW);
  
WiFiSetup();

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

}


void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    delay(1);
    WiFiSetup();
    }
  
  client.loop();
  PIR();
  
DoorOpen();
}

// VOIDS

void WiFiSetup()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(WiFi.localIP());

   if (client.connect("arduinoClient"))
  {
    //client.publish("/DEBUG/","ESP Connected");
    client.publish("/testsw/",(TopicPIR));
    client.publish("/testsw/",(TopicDOOR));
    client.subscribe("/testsw/#");
    Serial.println("MQTT Connected");
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }
  }

void PIR()
{
   if (client.connect("arduinoClient"))
  {
     if(digitalRead(pirPin) == HIGH){
       if(lockLow){  
         //makes sure we wait for a transition to LOW before any further output is made:
         lockLow = false;            
         Serial.println("---");
         Serial.print("motion detected at ");
         Serial.print(millis()/1000);
         Serial.println(" sec"); 
         client.publish(TopicPIR,"Motion");
   
         delay(50);
         }         
         takeLowTime = true;
       }

     if(digitalRead(pirPin) == LOW){       

       if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
       //if the sensor is low for more than the given pause, 
       //we assume that no more motion is going to happen
       if(!lockLow && millis() - lowIn > pause){  
           //makes sure this block of code is only executed again after 
           //a new motion sequence has been detected
           lockLow = true;                        
           Serial.print("motion ended at ");      //output
           Serial.print((millis() - pause)/1000);
           Serial.println(" sec");
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
  // handle message arrived
}
