

//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "painlessMesh/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "painlessMesh/to/broadcast".
// When you publish "getNodes" to "painlessMesh/to/gateway" you receive the mesh topology as JSON
// Every message from the mesh which is send to the gateway node will be published to "painlessMesh/from/12345678" where 12345678
// is the nodeId from which the packet was send.
//************************************************************

#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555



// Prototypes
void receivedCallback(const uint32_t &from, const String &msg);


IPAddress getlocalIP();

IPAddress myIP(0, 0, 0, 0);
IPAddress mqttBroker(192, 168, 1, 4);
Scheduler userScheduler;
painlessMesh mesh;
WiFiClient wifiClient;


void sendMsgToBroker();
Task taskSendMessage( TASK_SECOND * 3 , TASK_FOREVER, &sendMsgToBroker );


void setup()
{
  Serial.begin(115200);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION); // set before init() so that you can see startup messages

  // Channel set to 6. Make sure to use the same channel for your mesh and for you other
  // network (STATION_SSID)
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, 6);
  mesh.onReceive(&receivedCallback);
 mesh.setContainsRoot(true);
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

}

void loop()
{
  mesh.update();


  if (myIP != getlocalIP())
  {
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

  }
}

void sendMsgToBroker(){
  String msg = "Hello from node ";
  // msg += mesh.getNodeId();

  // Serial.printf("test");
  uint32_t target = 1375462669;
   mesh.sendSingle(target, msg);
}




void receivedCallback(const uint32_t &from, const String &msg)
{
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
}

IPAddress getlocalIP()
{
  return IPAddress(mesh.getStationIP());
}
