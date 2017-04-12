#include <SoftwareSerial.h>
SoftwareSerial wifitoserver(3, 2); 
bool flag=true;
String deviceID;
int _ID[4];
char device_ctr;
int index_pin=0;
static int char_count = 0;
int a=20;

typedef enum { 
  input, servomotor, pwm, digital } 
pinType;

typedef struct pin {
  pinType type;       //Type of pin
  int state;         //State of an output
  //byte value;       //Value of an input. Not used by now. TODO
};

pin arduinoPins[14];  //Array of struct holding 0-13 pins information

unsigned long lastDataReceivedTime = millis();

void setup()
{
  Serial.begin(38400);
  Serial.flush();
  wifitoserver.begin(9600);
  wifitoserver.flush();
  configurePins();
  resetPins();
 
}
static char buffer[5];
void loop()
{
//  return;
  static unsigned long timerCheckUpdate = millis();

  if (millis()-timerCheckUpdate>=20)
  {
    sendUpdateServomotors();
    sendSensorValues();
    timerCheckUpdate=millis();
  }

  readSerialPort();
}

void configurePins()
{
  arduinoPins[0].type=input;
  arduinoPins[1].type=input;
  arduinoPins[2].type=input;
  arduinoPins[3].type=input;
  arduinoPins[4].type=servomotor;
  arduinoPins[5].type=pwm;
  arduinoPins[6].type=pwm;
  arduinoPins[7].type=servomotor;
  arduinoPins[8].type=servomotor;
  arduinoPins[9].type=pwm;
  arduinoPins[10].type=digital;
  arduinoPins[11].type=digital;
  arduinoPins[12].type=digital;
  arduinoPins[13].type=digital;
}

void resetPins() {
  for (byte index=0; index <=13; index++) 
  {
    if (arduinoPins[index].type!=input)
    {
      pinMode(index, OUTPUT);
      if (arduinoPins[index].type==servomotor)
      {
        arduinoPins[index].state = 255;
        servo (index, 255);
      }
      else
      {
        arduinoPins[index].state=0;
        digitalWrite(index,LOW);
      }
    }
  }
}

void sendSensorValues()
{
  unsigned int sensorValues[6], readings[5];
  byte sensorIndex;
  
//   if ((char_count = wifitoserver.available()) > 0) {
//      int i;
//  
//        for (i = 0; i < 4; ++i)
//            buffer[i] = wifitoserver.read();
//        buffer[i] = '\0';

    sensorValues[0]=a;//sarah
    //Serial2.print(buffer[2]);
    //Serial2.flush();
    //for(int i=1;i<sizeof(buffer);i++){ buffer[i]={0}; } //sarah
    
//    delay(100);
//    b_flag=0;
  //}
  for (sensorIndex =1; sensorIndex < 6; sensorIndex++) //for analog sensors, calculate the median of 5 sensor readings in order to avoid variability and power surges
  {
    for (byte p = 0; p < 5; p++)
      readings[p] = analogRead(sensorIndex);
    insertionSort(readings, 5); //sort readings
    sensorValues[sensorIndex] = readings[2]; //select median reading
  }

  //send analog sensor values
  for (sensorIndex = 0; sensorIndex < 6; sensorIndex++)
    ScratchBoardSensorReport(sensorIndex, sensorValues[sensorIndex]);

  //send digital sensor values
  ScratchBoardSensorReport(6, digitalRead(2)?1023:0);
  ScratchBoardSensorReport(7, digitalRead(3)?1023:0);
}

void insertionSort(unsigned int* array, unsigned int n)
{
  for (int i = 1; i < n; i++)
    for (int j = i; (j > 0) && ( array[j] < array[j-1] ); j--)
      swap( array, j, j-1 );
}

void swap(unsigned int* array, unsigned int a, unsigned int b)
{
  unsigned int temp = array[a];
  array[a] = array[b];
  array[b] = temp;
}

void ScratchBoardSensorReport(byte sensor, int value) //PicoBoard protocol, 2 bytes per sensor
{
  Serial.write( B10000000
    | ((sensor & B1111)<<3)
    | ((value>>7) & B111));
  Serial.write( value & B1111111);
}

void readSerialPort()
{
  byte pin;
  int newVal;
  static byte actuatorHighByte, actuatorLowByte;
  static byte readingSM = 0;

  if (Serial.available())
  {
    if (readingSM == 0)
    {
      actuatorHighByte = Serial.read();
      if (actuatorHighByte >= 128) readingSM = 1;
    }
    else if (readingSM == 1)
    {
      actuatorLowByte = Serial.read();
      if (actuatorLowByte < 128) readingSM = 2;
      else readingSM = 0;
    }

    if (readingSM == 2)
    {
      lastDataReceivedTime = millis();    
      pin = ((actuatorHighByte >> 3) & 0x0F);
      newVal = ((actuatorHighByte & 0x07) << 7) | (actuatorLowByte & 0x7F); 
       if(pin==9 && newVal ==255)
          {
            if(flag==false)
              {
                deviceID="";
                for(int i=0; i<4;i++)
                  _ID[i]=0;
              }
              
              flag=true;
          }
      if(pin==9 && newVal ==200)
          {
            device_ctr='1'; //on
            if(flag)
              {
                 for(int i=0; i<4;i++)   
                   {   index_pin=i+10;             
                     _ID[i]= arduinoPins[index_pin].state;
                   }
                device_control (_ID,device_ctr);
                flag=false;
              }
            }
     if(pin==9 && newVal ==100) //sarah 0317
        {
          device_ctr='0'; //off
          if(flag)
              {
                
                for(int i=0; i<4;i++)   
                     { index_pin=i+10;             
                       _ID[i]= arduinoPins[index_pin].state;
                     }
                device_control (_ID,device_ctr);
                flag=false;
              }
          }
      if(arduinoPins[pin].state != newVal)
      {
        arduinoPins[pin].state = newVal;
        updateActuator(pin);
      }
      readingSM = 0;
    }
  }
  else checkScratchDisconnection();
}

void reset() //with xbee module, we need to simulate the setup execution that occurs when a usb connection is opened or closed without this module
{
  resetPins();        // reset pins
  sendSensorValues(); // protocol handshaking
  lastDataReceivedTime = millis();
}

void updateActuator(byte pinNumber)
{
  if (arduinoPins[pinNumber].type==digital) digitalWrite(pinNumber, arduinoPins[pinNumber].state);
  else if (arduinoPins[pinNumber].type==pwm) analogWrite(pinNumber, arduinoPins[pinNumber].state);
}

void sendUpdateServomotors()
{
  for (byte p = 0; p < 10; p++)
    if (arduinoPins[p].type == servomotor) servo(p, arduinoPins[p].state);
}

void servo (byte pinNumber, byte angle)
{
  if (angle != 255)
    pulse(pinNumber, (angle * 10) + 600);
}

void pulse (byte pinNumber, unsigned int pulseWidth)
{
  digitalWrite(pinNumber, HIGH);
  delayMicroseconds(pulseWidth);
  digitalWrite(pinNumber, LOW);
}

void checkScratchDisconnection() //the reset is necessary when using an wireless arduino board (because we need to ensure that arduino isn't waiting the actuators state from Scratch) or when scratch isn't sending information (because is how serial port close is detected)
{
  if (millis() - lastDataReceivedTime > 1000) reset(); //reset state if actuators reception timeout = one second
}
void recv_show(char dev_Id[]){
  //Ascii convert to binaray 
  //char *show = dev_Id;
  //unsigned long show_id=strtoul(show,NULL,16);
  //char show_id[5];
  //String(dev_Id, BIN).toCharArray(show_id,5);
//  for(int i=4;i<8;i++){
//    if(){}
    //analogWrite(0,);
    
//    }
  }
void device_control (int _ID[], char turn)//Sarah 001
{ 
  char msg[5];
  char *p=msg;
  for(int i=0; i<4;i++)
   { deviceID += _ID[i];
//      buffer[i] =_ID[i];
   }
   deviceID.toCharArray(msg,10);
   unsigned long _msglong=strtoul(p, NULL, 2);
   for(int i=1;i<sizeof(msg);i++)
    {
      msg[i]={0};
      }
    String(_msglong, HEX).toCharArray(msg,10);
    msg[1]= turn;
    wifitoserver.print(msg);
}


