//test ack pin10 with sensor on
#include <SoftwareSerial.h>
SoftwareSerial showmsg(3, 2);
byte pulldown_ack = 1; //A1
byte msg = 2; //A2 msg
byte handshaking = 0;
int the_value = 255;

byte scratch_ack = 10;
typedef enum {
  input, servomotor, pwm, digital
}
pinType;

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
  showmsg.begin(9600);
  showmsg.flush();
  delay(10);
  configurePins();

}

void loop()
{
  readSerialPort();
  delay(10);
}
//
void configurePins()
{
  for (int index = 7; index < 10; index++)
  {
    arduinoPins[index].type = digital;
    arduinoPins[index].state = 0;
    pinMode(index, OUTPUT);
    digitalWrite(index, LOW); //reset pins
    
  }
   pinMode(10, INPUT_PULLUP);
}

void ScratchBoardSensorReport(byte sensor, int value) //PicoBoard protocol, 2 bytes per sensor
{
  Serial.write( B10000000
                | ((sensor & B1111) << 3)
                | ((value >> 7) & B111));
  Serial.write( value & B1111111);
}

void readSerialPort()
{
  ScratchBoardSensorReport(handshaking, 0);
  int pin, inByte, sensorHighByte, states;

  if (Serial.available() > 1)
  {
    lastDataReceivedTime = millis();
    inByte = Serial.read();
    if (inByte >= 128) // Are we receiving the word's header?
    {
      sensorHighByte = inByte;
      pin = ((inByte >> 3) & 0x0F);
      showmsg.println(pin);
      if (pin == scratch_ack) {
        while (!Serial.available()); // Wait for the end of the word with data
        inByte = Serial.read();
        
        if (inByte <= 127) // This prevents Linux ttyACM driver to fail
        {

          states = ((sensorHighByte & 0x07) << 7) | (inByte & 0x7F);
          
          if (states == 1) {
            the_value = 1;
            digitalWrite(pin, states);
            Serial.write( B10000000
                          | ((pulldown_ack & B1111) << 3)
                          | ((the_value >> 7) & B111));//why not &B1就好?
            Serial.write( the_value & B1111111);
            showmsg.println("Pin on");
            
          }
          else {
            the_value = 0;
            digitalWrite(pin, states);
            Serial.write( B10000000
                          | ((pulldown_ack & B1111) << 3)
                          | ((the_value >> 7) & B111));//why not &B1就好?
            Serial.write( the_value & B1111111);
            
            showmsg.println("Pin off");
            
          }
        }
      }
      else {
        while (!Serial.available()); // Wait for the end of the word with data
        inByte = Serial.read();
        if (inByte <= 127) // This prevents Linux ttyACM driver to fail
        {
          states = ((sensorHighByte & 0x07) << 7) | (inByte & 0x7F);
          digitalWrite(pin, states);
        }
      }
    }
  }
  else checkScratchDisconnection();
}

void reset() //with xbee module, we need to simulate the setup execution that occurs when a usb connection is opened or closed without this module
{
  configurePins();
  ScratchBoardSensorReport(handshaking, 0);
  lastDataReceivedTime = millis();
}
