/*
 *  This sketch demonstrates how to scan WiFi networks. 
 *  The API is almost the same as with the WiFi Shield library, 
 *  the most obvious difference being the different file you need to include:
 *  Comes with Arduino ESP8266 build under examples..
 
 Modified by DanBicks added OLED display to ESP01 using cheap Ebay 0.96" display
 Lightweight Mike-Rankin OLED routines added in this project, Credits to Mike for this.
 
 Display indicates WIFI Scan operation, looks for specific access point,
 connects to this if found and displays network connection details on OLED.
 
 Simple guide for users on how OLED can be implemented in to your project.
 My contribution to this superb community.
 
 Keep up the fantastic work everyone :)
 
 I hope you enjoy this sketch.
 
 Dans

 TEMPLATE FOR WRITING TEXT// 16 Character max per line with font set
  
  clear_display();
  sendStrXY("Connecting to:",0,1);
  sendStrXY((ssid),2,1); 
  sendStrXY("Using Password:",4,1); 
  sendStrXY((password),6,1); 
 
 */
 
#include <PubSubClient.h>
#include "ESP8266WiFi.h"
#include <Wire.h>
#include "font.h"


// DOOR DEFINES //

// Change Topic to the topic you want to control
char* TopicDOOR = "/room/door/";

int buttonPin = 5;
boolean currentState = LOW;//stroage for current button state
boolean lastState = LOW;//storage for last button state
boolean doorState = LOW;//storage for the current state of the LED (off/on)


// WiFi DEFINES //
char* ssid = "..........";           // Router SSID
char* password = "..........";       // Router Passcode
char* server = "..........";         // Mosquitto Server IP

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

// OLED DEFINES //

#define OLED_address  0x3C  //OLED I2C bus address

char buffer[20]; 
extern "C" {
#include "user_interface.h"
}

void callback(char* topic, byte* payload, unsigned int length) {

  if (strcmp(topic,"/home/1/ard1/p1/com")==0) { 
   if (payload[0] == '0') 
    {
    digitalWrite(4, LOW);
  Serial.println("Turning Fan OFF");
  clear_display();
  sendStrXY("Turning Fan OFF",2,1); 

    delay(100);
    client.publish("/home/1/ard1/p1/state","0");
    } 
    else if (payload[0] == '1') 
    {
    digitalWrite(4, HIGH);
  Serial.println("Turning Fan ON");  
  clear_display();
  sendStrXY("Turning Fan ON",2,1); 

    delay(100);
    client.publish("/home/1/ard1/p1/state","1");
    }
   } 

  if (strcmp(topic,"/home/1/ard1/p2/com")==0) { 
   if (payload[0] == '0') 
    {
    digitalWrite(5, LOW);
  Serial.println("Turning Light OFF");  
  clear_display();
  sendStrXY("Turning Light OFF",2,1); 

    delay(100);
    client.publish("/home/1/ard1/p2/state","0");
    } 
    else if (payload[0] == '1') 
    {
    digitalWrite(5, HIGH);
  Serial.println("Turning Light ON");  
  clear_display();
  sendStrXY("Turning Light ON",2,1); 

    delay(100);
    client.publish("/home/1/ard1/p2/state","1");
    }
   } 


}

/*
------------------------------------------------------------------------------------
                     Setup routines all located here!
------------------------------------------------------------------------------------
*/

void setup() {
  
  Serial.begin(9600);
  delay(2000); // wait for uart to settle and print Espressif blurb..
  
  // print out all system information
  Serial.print("Heap: "); Serial.println(system_get_free_heap_size());
  Serial.print("Boot Vers: "); Serial.println(system_get_boot_version());
  Serial.print("CPU: "); Serial.println(system_get_cpu_freq());
 
  Serial.println();
  Serial.println("OLED network Acquire Application Started....");
  
   //Initialize I2C and OLED Display
  Wire.pins(0, 2); // SDA - GPIO0 - D3 _ SCL GPIO2 - D$
  Wire.begin();
  StartUp_OLED(); // Init Oled and fire up!
  Serial.println("OLED Init...");
  
  clear_display();
  sendStrXY("   PAULHAM ",0,1); // 16 Character max per line with font set
  sendStrXY("Home Automation",2,1); 
  sendStrXY("FAN / RELAY",4,1); 
  sendStrXY("START-UP ....  ",6,1); 
  
  delay(2000);

  Serial.println("Setup done");

 // RELAY SETUP
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  
  WiFiSetup();
 
delay(100);

}


/*
------------------------------------------------------------------------------------
                         Main Loop all loops here!
------------------------------------------------------------------------------------
*/

void loop() 

{

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connection Lost: Attempting to reconnect ");
    delay(1);
    WiFiSetup();
    return;
    }
    
  client.loop();
  
}


void WiFiSetup()
{
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  clear_display();
  sendStrXY("Connecting to:",2,1); // 16 Character max per line with font set
  sendStrXY((ssid),4,1); 

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


  clear_display();
  sendStrXY("Connected to",0,1);
  sendStrXY((ssid),2,1); 
  sendStrXY("Connecting MQTT",6,1);


  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(WiFi.localIP());

   if (client.connect("arduinoClient"))
  {
    client.publish("/DEBUG/","ESP Connected");
    client.subscribe("/DEBUG/#");
    Serial.println("MQTT Connected");
    
  clear_display();
  IPAddress ip = WiFi.localIP(); // // Convert IP Here 
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  ipStr.toCharArray(buffer, 20);   
  sendStrXY("Connected to",0,1);
  sendStrXY((ssid),2,1); 
  sendStrXY((buffer),4,1); //Print IP address to LCD 
  sendStrXY("MQTT Connected",6,1); 

  
  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
    
      clear_display();
  IPAddress ip = WiFi.localIP(); // // Convert IP Here 
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  ipStr.toCharArray(buffer, 20);   
  sendStrXY("Connected to",0,1);
  sendStrXY((ssid),2,1); 
  sendStrXY((buffer),4,1); //Print IP address to LCD 
  sendStrXY("MQTT FAIlED",6,1); 
  }
  }


