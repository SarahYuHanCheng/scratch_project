//test ack pin10 with sensor on
#include <SoftwareSerial.h>
SoftwareSerial wifitoserver(3, 2);
byte handshaking_pin = 0;//A0
byte msg_ID_pin = 2; //A2 msg
byte msg_act_pin = 1; //A1
byte act_back = 3; //A3

int count = 0;
int the_value;
int msg_ID = 0;
int old_msg_ID = 1;
byte scratch_ack = 10;

bool flag = true;
static int char_count = 0;
char cat[10];
String deviceID;
int _ID[0];
char device_ctr;

typedef enum {  digital//  input, servomotor, pwm,
             } pinType;

typedef struct pin {
  pinType type;       //Type of pin
  int state;         //State of an output
};

pin arduinoPins[14];  //Array of struct holding 0-13 pins information

unsigned long lastDataReceivedTime = millis();
void setup()
{
  Serial.begin(38400);
  Serial.flush();
  wifitoserver.begin(9600);
  wifitoserver.flush();
  delay(10);
  configurePins();
}

void loop()
{
  recvwifi();
  delay(10);
}
void recvwifi() {
  if ((char_count = wifitoserver.available()) > 0) { //recv msg from server

    old_msg_ID = msg_ID;
    delay(10);
    for (int i = 0; i < char_count; ++i) {
      cat[i] = wifitoserver.read(); //char mssg[]
      if (i == 0) {
        msg_ID = strtoul(cat, NULL, 16); //ID


      }
      delay(30);
      if (msg_ID != old_msg_ID) {
        count = 0;
      }
    }
    for (int i = 1; i < sizeof(cat); i++) {
      cat[i] = {0};
    }

  }
  delay(20);//for the first time after reset ID to 0
  ScratchBoardSensorReport(msg_ID_pin, msg_ID);
  count++;

  if (count > 280) {
    msg_ID = 0; count = 0;
    lastDataReceivedTime = millis();
  }//in 10sec, the msg is the same from same client
  // after 10sec, set IID to 0, so the msg from the same client would be recog
  delay(10);
}

void configurePins()
{
  for (int index = 7; index < 14; index++)
  {
    arduinoPins[index].type = digital;
    arduinoPins[index].state = 0;
    pinMode(index, OUTPUT);
    digitalWrite(index, LOW); //reset pins
  }
}

void ScratchBoardSensorReport(byte sensor, int value) //PicoBoard protocol, 2 bytes per sensor
{
  Serial.write( B10000000
                | ((sensor & B1111) << 3)
                | ((value >> 7) & B111));
  Serial.write( value & B1111111);
}

void checkScratchDisconnection()
{
  if (millis() - lastDataReceivedTime > 1000) reset(); //reset state if actuators reception timeout = one second
}
void reset() //with xbee module, we need to simulate the setup execution that occurs when a usb connection is opened or closed without this module
{
  configurePins();
  ScratchBoardSensorReport(handshaking_pin, 0);
  lastDataReceivedTime = millis();
}
void device_control (int _ID[], char turn)//Sarah 001
{
  deviceID += String( _ID[0], HEX);
  deviceID += turn;
  wifitoserver.print(deviceID);
}
