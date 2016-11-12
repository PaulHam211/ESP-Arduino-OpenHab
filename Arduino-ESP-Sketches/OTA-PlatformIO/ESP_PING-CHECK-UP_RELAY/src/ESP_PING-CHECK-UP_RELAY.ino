// Library
#include <ESP8266WiFi.h>
#include "SSIDPASS.h"
#include "OTA.h"

// Startup variables
#define startDelay 120000 // Giving the router 2 minutes to start up

// Pin configuration
#define relayPin 12
#define ledPin 13
#define buttonPin 0

// Server to open a connection to
char serverAdr[] = "192.168.1.10";
#define serverPort 8080
#define interval 300000 // Try to connect every 5 minutes

WiFiClient wifiClient;

unsigned long prevTime = 0;
unsigned long healthLed = 0;
unsigned long routerReset = 0;
unsigned long intervalmin = interval / 60000; // Convert milliseconds to seconds

int buttonState = 0;         // variable for reading the pushbutton status


void setup()
{
  OTA_Setup();

  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Turn the relay on
  digitalWrite(relayPin, HIGH);

  Serial.begin(9600);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");

    digitalWrite(ledPin, !digitalRead(ledPin)); // Rapid flashing of the led to show we are trying to connect

    if(millis() > startDelay && routerReset == 0) // Wait 120 seconds for the router to start up and hand out an ip
    {
      Serial.println("WiFi connection failed, restarting.");
      routerReset = 1;
    }

    routerResetRoutine();
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop()
{
  OTA_Loop();

  // WiFi connection lost while running, reset to get back to the startup routine
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi connection lost, restarting.");
    ESP.restart();
  }

  if (millis() - prevTime >= interval) // Try to contact the server when time has run out
  {
    prevTime = millis();
    httpGET();
  }

  if (millis() - healthLed >= 2000) // Slow flashing of the led to show we are connected and monitoring
  {
    healthLed = millis();
    digitalWrite(ledPin, !digitalRead(ledPin));
  }

 buttonState = digitalRead(buttonPin);
 if (buttonState == LOW) {
   routerReset = 1;
   delay(1000);
 }

  routerResetRoutine();
}

void httpGET()
{
  Serial.print("\nStarting connection to server - ");
  Serial.print(serverAdr);
  Serial.print(" at Port - ");
  Serial.println(serverPort);

  Serial.print("Connection: ");
  if (wifiClient.connect(serverAdr, serverPort))
  {
    Serial.println("OK");
    Serial.print("Trying again in ");
    Serial.print(interval);
    Serial.print(" milliseconds, ");
    Serial.print(intervalmin);
    Serial.println(" minutes");

    wifiClient.stop();
  }
  else
  {
    Serial.println("FAILED");
    routerReset = 1;
  }
}




void routerResetRoutine()
{
  if (routerReset == 1)
  {
    Serial.println("Switching router off");
    routerReset = millis();
    digitalWrite(relayPin, LOW);
  }

  if (millis() - routerReset >= 10000 && routerReset > 1)
  {
    Serial.println("Restarting...");
    ESP.restart();
  }
}
