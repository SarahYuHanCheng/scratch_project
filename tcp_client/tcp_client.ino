#include <ESP8266WiFi.h>

#include <WiFiClient.h>

/* A TCP client.
 */
WiFiClient wifiClient;

 // You have to modify the following parameters.
#define SSID "scream"
#define PASSWD "s741852scream"
#define TCP_IP "192.168.1.205"
#define TCP_PORT 8888

#define deviceID 12
int ledpin =D5;
char _buffer[3];

void setup()
{
   pinMode(ledpin, OUTPUT);
   pinMode(2, OUTPUT);
  int idflag =1;
  char joinedSSID[32];

  // Initialize the UART
  Serial.begin(9600);
  while (!Serial)
    ;
   
   Serial.println("Connecting...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        WiFi.begin(SSID, PASSWD);
        Serial.println("Retrying...connect to AP");
    }
    Serial.println("Connected to AP");
     wifiClient.connect(TCP_IP, TCP_PORT);
     _buffer[0]='S';
      String(deviceID, HEX).toCharArray((_buffer+1), 2);
      wifiClient.write(_buffer,2);
     Serial.println("sended id");
  
}

static char buffer[32];
static unsigned long timerCheckUpdate = millis();
void loop()
{
  int charAvail;
  
  // gets() would return the ID of the sender.
  if ((charAvail = wifiClient.available()) >0) {
    wifiClient.read((unsigned char *)buffer, 32);
      if(buffer[0]=='1'){ digitalWrite(2,HIGH);//BUILTIN_LED off
      Serial.println(buffer[0]);}
      else if(buffer[0]=='0'){digitalWrite(2,LOW);}//BUILTIN_LED on
      
      for(int i=1;i<sizeof(buffer);i++){ buffer[i]={0}; }
  }
  
  if (millis()-timerCheckUpdate>=10000)
  {
        if(!wifiClient.connected()){
           // Cannot join to the TCP server, stop.
           if(!wifiClient.connect(TCP_IP, TCP_PORT)){
              Serial.println("connection failed");
               int i=0;
               while(i<2){
                 digitalWrite(ledpin,HIGH);
                 delay(800);
                 digitalWrite(ledpin,LOW);
                 delay(200);
                 i++;
               }
              //return; 
            }else{
             _buffer[0]='S';
              String(deviceID, HEX).toCharArray((_buffer+1), 2);
              wifiClient.write(_buffer,2);
             Serial.println("sended id again");
            }
        }
    timerCheckUpdate=millis();
  }
}
