#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#define SSID "Connectify-ptw"
#define PASSWD "screamlab"
#define TCP_IP "192.168.179.1"
#define TCP_PORT 8888

#define deviceID 1

WiFiClient wifiClient;
int wifi_connect = 5;
char _buffer[3]; //sarah0411
void setup()
{

  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWD);
    Serial.println("Retrying...");
    delay(100);
  }
  Serial.println("Connected to AP");

  wifiClient.connect(TCP_IP, TCP_PORT);
  _buffer[0] = 'X';
  String(deviceID, HEX).toCharArray((_buffer + 1), 2);
  //      _buffer[1]=deviceID;
  wifiClient.write(_buffer, 2);
  for (int i = 1; i < sizeof(_buffer); i++) {
    _buffer[i] = {0};
  }
}

static char buffer[32];
static int char_count = 0;
static unsigned long timerCheckConnectWifi = millis();
void loop()
{
  if (millis() - timerCheckConnectWifi >= 10000)
  {
    checkconnect();
  }
  if ((char_count = Serial.available()) > 0) {
    int i;
    buffer[0] = 'W';
    for (i = 1; i < char_count + 1; ++i)
      buffer[i] = Serial.read();
    buffer[i] = '\0';


    //        wifiClient.write(buffer, char_count+1);
    //        wifiClient.flush();
    //        Serial.write(buffer, char_count+1);
    //        Serial.flush();
    for (int i = 1; i < sizeof(buffer); i++) {
      buffer[i] = {0};  //sarah
    }
    if (strstr(buffer, "end")) {
      wifiClient.stop();
      while (1)
        ;
    }
  }
  if ((char_count = wifiClient.available()) > 0) {
    timerCheckConnectWifi = millis();
    wifiClient.read((unsigned char *)buffer, 32);
    Serial.println(buffer);
    for (int i = 1; i < sizeof(buffer); i++) {
      buffer[i] = {0};  //sarah
    }
  }
}

void checkconnect() {


  if (!wifiClient.connected()) {
    // Cannot join to the TCP server, stop.
    if (!wifiClient.connect(TCP_IP, TCP_PORT)) {
      while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        WiFi.begin(SSID, PASSWD);
        delay(100);
      }
      int i = 0;
      while (i < 2) {
        digitalWrite(wifi_connect, HIGH);
        delay(800);
        digitalWrite(wifi_connect, LOW);
        delay(200);
        i++;
      }
    } else {
      _buffer[0] = 'X';
      String(deviceID, HEX).toCharArray((_buffer + 1), 2);
      wifiClient.write(_buffer, 2);
      for (int i = 1; i < sizeof(_buffer); i++) {
        _buffer[i] = {0};
      }
    }
  } else {
    timerCheckConnectWifi = millis();
  }

}
