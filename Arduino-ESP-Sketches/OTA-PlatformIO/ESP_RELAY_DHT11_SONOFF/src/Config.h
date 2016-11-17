

// Mosquitto Server IP
  char const* server = "192.168.1.10";

// Client/Device ID for Mosquitto
  char const* clientid = "GardenDHTRELAY";

// Change Topic to the topic you want to control
  char const* TopicTEMP = "/Garden/Temp/";
  char const* TopicHUM = "/Garden/Humidity/";

  char const* TopicCOMRELAY1 = "/Garden/relay/1/com/";
  char const* TopicSTATERELAY1 = "/Garden/relay/1/state/";

// Debug message to send on sucessful connection
  char const* debugmess = "Garden DHT-Relay Connected On ";

/*
//TEST

// Client/Device ID for Mosquitto
  char const* clientid = "test1";

// Change Topic to the topic you want to control
  char const* TopicTEMP = "/temp/Temp/";
  char const* TopicHUM = "/temp/Humidity/";

  char const* TopicCOMRELAY1 = "/relay/1/com/";
  char const* TopicSTATERELAY1 = "/relay/1/state/";

// Debug message to send on sucessful connection
  char const* debugmess = "test garden Connected On ";
  */
