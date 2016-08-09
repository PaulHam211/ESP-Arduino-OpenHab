

// Mosquitto Server IP
  char const* server = "192.168.1.10";

// Client/Device ID for Mosquitto
  char const* clientid = "FrontDoor1";
  
// Topic Defines //
  const char* TopicDOOR = "/test/frontdoor/mag/";  // Topic to publih message to

  char const* TopicCOMRELAY1 = "/test/frontdoor/relay/1/com/";        // Topic READ to publish message from
  char const* TopicSTATERELAY1 = "/test/frontdoor/relay/1/state/";    // Topic state to verify message received
  
// Debug message to send on sucessful connection
  char const* debugmess = "Front Door test Connected";


