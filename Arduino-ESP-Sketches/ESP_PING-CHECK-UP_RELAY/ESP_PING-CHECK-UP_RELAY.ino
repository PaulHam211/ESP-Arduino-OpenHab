// WiFi info
char const* ssid = "BTHub5-ZG53";           // Router SSID
char const* password = "59d49ee597";       // Router Passcode
 
 
// Startup variables
#define startDelay 120000 // Giving the router 2 minutes to start up
 
 
// Pin configuration
#define relayPin 12
#define ledPin 13
 
 
// Server to open a connection to
char serverAdr[] = "192.168.1.10";
#define serverPort 8080
#define interval 300000 // Try to connect every 5 minutes
 
 
// Library
#include <ESP8266WiFi.h>
 
 
WiFiClient wifiClient;
 
unsigned long prevTime = 0;
unsigned long healthLed = 0;
unsigned long routerReset = 0;
 
void setup()
{
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
 
  // Turn the relay on
  digitalWrite(relayPin, HIGH);
   
  Serial.begin(9600);
  Serial.println("");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
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
 
  routerResetRoutine();
}
 
void httpGET()
{
  Serial.print("\nStarting connection to server - ");
  Serial.print(serverAdr);
  Serial.print("at Port - ");
  Serial.println(serverPort);

  Serial.print("Connection: ");  
  if (wifiClient.connect(serverAdr, serverPort))
  {
    Serial.println("OK");
    Serial.print("Trying again in ");
    Serial.print(interval);
    Serial.println(" milliseconds");

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
