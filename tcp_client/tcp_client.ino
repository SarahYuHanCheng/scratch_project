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
static String deviceID="08";

int in_msg =D0;
int wifi_connect=D5;
int out_msg =D2;
char _buffer[4];

void setup()
{
   pinMode(in_msg, OUTPUT);
   pinMode(wifi_connect, OUTPUT);
   
   
   pinMode(out_msg, INPUT_PULLUP);
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
      deviceID.toCharArray((_buffer+1), 3);
       wifiClient.write(_buffer,3);
      for(int i=1;i<sizeof(_buffer);i++){ _buffer[i]={0}; }
     Serial.println("sended id");
  
}

static char buffer[32];
static unsigned long timerCheckUpdate = millis();
static unsigned long timer ;
void loop()
{
  int charAvail;
  int temp=digitalRead(out_msg);
  delay(200);//0.5 sec
  int temp2=digitalRead(out_msg);
  if(!temp && (temp!=temp2)){
    deviceID.toCharArray((_buffer), 3);
    _buffer[2]='1';
     Serial.print("deviceID:");
      Serial.println(deviceID);
     Serial.print("_buffer:");
      Serial.println(_buffer);
    wifiClient.write(_buffer,3);
    for(int i=1;i<sizeof(_buffer);i++){ _buffer[i]={0}; }   
    
    }
  // gets() would return the ID of the sender.
  if ((charAvail = wifiClient.available()) >0) {
    wifiClient.read((unsigned char *)buffer, 32);
      if(buffer[0]=='1'){ digitalWrite(in_msg,HIGH);//BUILTIN_LED off
      Serial.println(buffer[0]);}
      else if(buffer[0]=='0'){digitalWrite(in_msg,LOW);}//BUILTIN_LED on
      
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
                 digitalWrite(wifi_connect,HIGH);
                 delay(800);
                 digitalWrite(wifi_connect,LOW);
                 delay(200);
                 i++;
               }
              //return; 
            }else{
             _buffer[0]='S';
              deviceID.toCharArray((_buffer+1), 3);
      wifiClient.write(_buffer,3);
              for(int i=1;i<sizeof(_buffer);i++){ _buffer[i]={0}; }
             Serial.println("sended id again");
            }
        }
    timerCheckUpdate=millis();
  }
}
