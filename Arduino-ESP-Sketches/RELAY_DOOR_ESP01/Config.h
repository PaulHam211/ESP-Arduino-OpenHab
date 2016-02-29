

// Mosquitto Server IP
  char const* server = "..........";

// Client/Device ID for Mosquitto
  char const* clientid = "FrontDoor";
  
// Topic Defines //
  const char* TopicDOOR = "/os/frontdoor/mag/";  // Topic to publih message to

  char const* TopicCOMRELAY1 = "/os/frontdoor/relay/1/com/";        // Topic READ to publish message from
  char const* TopicSTATERELAY1 = "/os/frontdoor/relay/1/state/";    // Topic state to verify message received
  
// Debug message to send on sucessful connection
  char const* debugmess = "Front Door Connected";


