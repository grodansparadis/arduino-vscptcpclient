/*
  This example connect to a remote VSCP host and display received events 
  using a callback
*/

//#include <GDBStub.h>    // Uncomment for debugger

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#include <vscptcpclient.h>

static const char VSCP_ETH_PREFIX[] PROGMEM = "FF:FF:FF:FF:FF:FF:FF:FE:";

char vscp_guid[50];

const char *ssid = ""; // Enter the SSID of your WiFi Network.
const char *password =
    ""; // Enter the Password of your WiFi Network.

// VSCP connection credentials
const char *vscp_server = "192.168.1.7";
const int16_t vscp_port = 9598;
const char *vscp_user = "admin";
const char *vscp_password = "secret";

WiFiClient espClient;
// WiFiClientSecure espClient;

// Prototypes
void dumpVscpEvent(vscpEventEx *pex);
void callback(vscpEventEx *pex);

vscpTcpClient vscp(vscp_server, vscp_port, callback, espClient);


///////////////////////////////////////////////////////////////////////////////
// setup
//

void setup() {

  strcpy(vscp_guid,"FF:FF:FF:FF:FF:FF:FF:FE:");
  strcat(vscp_guid,WiFi.macAddress().c_str());
  strcat(vscp_guid,":00:00");

  Serial.begin(115200);
  //gdbstub_init();     // Uncomment for debug
  delay(200); // Needs some time to stabilize

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println();
  Serial.println();
  Serial.print("Connecting To: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }

  Serial.println();
  Serial.println("WiFi Connected.");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("ESP Board MAC Address:  ");
  Serial.println("FF:FF:FF:FF:FF:FF:FF:FE:" + WiFi.macAddress() + ":00:00");
  Serial.println(vscp_guid);

  // Give system a chance to work.
  yield();

  // Connect to remote VSCP host
  if ( VSCP_ERROR_SUCCESS == vscp.connect("admin","secret") ) {
    Serial.printf("Connected to remote VSCP host. Waiting for events");
  }
  else {
    Serial.printf("Failed to connect to remote VSCP host");
  }

}

///////////////////////////////////////////////////////////////////////////////
// loop
//

void loop() 
{ 
  vscp.loop(); 
}

/////////////////////////////////////////////////////////////////////
// dumpVscpEvent
//

void dumpVscpEvent(vscpEventEx *pex)
{
  char buf[80];

  Serial.print("VSCP head=");
  Serial.println(pex->head);
  
  Serial.print("VSCP Class=");
  Serial.println(pex->vscp_class);
  
  Serial.print("VSCP Type=");
  Serial.println(pex->vscp_type);
  
  Serial.print("VSCP OBID=");
  Serial.println(pex->obid);

  vscp.writeDateTimeToStr(buf, *pex);
  Serial.print("VSCP DateTime=");
  Serial.println(buf);

  Serial.print("VSCP Timestamp=");
  Serial.println(pex->timestamp);

  Serial.print("VSCP GUID=");
  vscp.writeGuidToStr(buf,pex->GUID);
  Serial.println(buf);

  Serial.print("VSCP sizeData=");
  Serial.println(pex->sizeData);

  Serial.print("VSCP Data: ");
  for ( int j=0; j<pex->sizeData; j++ ) {
    Serial.print("0x");
    itoa(pex->data[j],buf,16);
    Serial.print(buf);
    Serial.print(" ");
  }

  Serial.println();
}

/////////////////////////////////////////////////////////////////////
// callback
//

void callback(vscpEventEx *pex) 
{
  dumpVscpEvent(pex);
  Serial.println("-------------------------------------------------");
  delete pex;
}
