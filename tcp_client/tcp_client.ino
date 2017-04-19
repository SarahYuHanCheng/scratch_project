/* A TCP client.
 */
#include <KSM111_ESP8266.h>
#include <SoftwareSerial.h>

 KSM111_ESP8266 wifi(3, 2);  // USe SoftwareSerial: Rx pin 3, Tx pin 2.


 // You have to modify the following parameters.
#define AP_SSID         "scream"
#define AP_PASSWD       "s741852scream"
#define TCP_SERVER_IP   "192.168.1.205"
#define TCP_SERVER_PORT 8888

#define deviceID 12
char _buffer[3];

void setup()
{
  
  int idflag =1;
  char joinedSSID[32];

  // Initialize the UART
  Serial.begin(9600);
  while (!Serial)
    ;
    pinMode(13,OUTPUT);
  wifi.begin(9600);

  Serial.print("Connecting to the AP... ");
  // Check if the module joined an AP.
  if (!wifi.joinedAP(joinedSSID) ||
      strcmp(joinedSSID, AP_SSID) != 0) {
    // Quit the current connected AP and connect to the new one.
    wifi.quitAP();
    wifi.setMode(STATION);
    wifi.multiConnect(false);
    if (wifi.joinAP(AP_SSID, AP_PASSWD) < 0) {
      // Cannot connect to the AP, stop.
      Serial.println("Fail");
      while (1)
        ;
    }
    
  }

  Serial.print("OK\nJoining to the TCP server... ");
  
  if (wifi.beginClient("TCP", TCP_SERVER_IP, TCP_SERVER_PORT) != CONNECT_ERROR)
    {
      delay(500);
      _buffer[0]='S';
      String(deviceID, HEX).toCharArray((_buffer+1), 2);
      wifi.puts(_buffer);
      Serial.println("sended id OK");
    }
  else {
    // Cannot join to the TCP server, stop.
    Serial.println("Fail");
    while (1)
      ;
  }
}

static char buffer[32];
static unsigned long timerCheckUpdate = millis();
void loop()
{
  int charAvail;
  
  // gets() would return the ID of the sender.
  if ((charAvail=wifi.gets(buffer, 8)) >= 0) {
      if(buffer[0]=='1'){ digitalWrite(13,HIGH);
      Serial.println(buffer[0]);}
      else if(buffer[0]=='0'){digitalWrite(13,LOW);}
      
      for(int i=1;i<sizeof(buffer);i++){ buffer[i]={0}; }
  }
  

  if (millis()-timerCheckUpdate>=10000)
  {
        if(!wifi.isClientConnected()){
          if (wifi.beginClient("TCP", TCP_SERVER_IP, TCP_SERVER_PORT) != CONNECT_ERROR)
          {
            _buffer[0]='S';
             String(deviceID, HEX).toCharArray((_buffer+1), 2);
            wifi.puts(_buffer);
          }
        else {
          // Cannot join to the TCP server, stop.
          Serial.println("Fail");
          while (1)
            ;
        }
      }else { 
        //Serial.println("is connected");
        }
    timerCheckUpdate=millis();
  }
}

