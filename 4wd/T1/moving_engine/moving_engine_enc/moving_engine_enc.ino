#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PWMServoDriver.h>
#include <EnableInterrupt.h>
#include "configuration.h"
#include "MoveEngines.h"
#include "rfid.h"

#define SERIAL_DEBUG_MODE true

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

#define ABSOLUTE_MAX_POWER 4095

int maxPower = ABSOLUTE_MAX_POWER;
int currentPower = maxPower;
int minPower = 2000;

// for input data from ble
boolean cleanupSerial;
bool isValidInput;
char inData[SERIAL_BUFFER]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

void setup()
{
    //enable serial
    Serial.begin(38400);
    //init the PWM driver
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);
    SPI.begin(); // Init SPI bus
    isValidInput = false;
    //enable encoders ISR
    pinMode(leftFrontEncoderPin, INPUT_PULLUP);
    pinMode(rightFrontEncoderPin, INPUT_PULLUP);
    pinMode(leftBackEncoderPin, INPUT_PULLUP);
    pinMode(rightBackEncoderPin, INPUT_PULLUP);
    enableInterrupt(leftFrontEncoderPin, isrLeftFrontEncoder, RISING);
    enableInterrupt(rightFrontEncoderPin, isrRightFrontEncoder, RISING);
    enableInterrupt(leftBackEncoderPin, isrLeftBackEncoder, RISING);
    enableInterrupt(rightBackEncoderPin, isrRightBackEncoder, RISING);
    cleanupSerial = false;
    maxPower=4000;
    engineSetup();
    if (hasRFID) {
      initRFID();
    }
}

boolean isValidNumber(char *data, int size)
{
  if (size == 0 ) return false;
   boolean isNum=false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++)
   {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

boolean makeCleanup() {
  for (index = 0; index < SERIAL_BUFFER; index++) {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
  Serial.flush();
}

bool setMaxPowerCommand(char *buffer) {
  sprintf(buffer,"OK\r\n");
  Serial.print(buffer);
  Serial.flush();
  //remove V from command
  for (uint8_t i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData, index - 2)) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if (atol(inData) > ABSOLUTE_MAX_POWER || atol(inData) < 0) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  maxPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool setMinPowerCommand(char *buffer) {
  sprintf(buffer,"OK\r\n");
  Serial.print(buffer);
  Serial.flush();
  //remove v from command
  for (uint8_t i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData, index - 2)) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if (atol(inData) > maxPower || atol(inData) < 0) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  minPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool setCurrentPowerCommand(char *buffer) {
  sprintf(buffer,"OK\r\n");
  Serial.print(buffer);
  Serial.flush();
  //remove c from command
  for (uint8_t i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData, index - 2)) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if (atol(inData) > maxPower || atol(inData) < 0) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  currentPower = atol(inData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool processPlatformUnsupoprtedCommand(char *buffer) {
  sprintf(buffer,"OK\r\n");
  Serial.print(buffer);
  Serial.flush();
  makeCleanup();
  isValidInput = true;
  return true;
}

bool moveOrRotateUntilNextCommand(char *buffer) {
  //remove M from command
  for (uint8_t i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  inData[strlen(inData)] = '\0';
  int position;
  for (uint8_t i = 0; i < strlen(inData); i++) {
    if (inData[i] == ',') {
      position = i;
      break;
    }
  }
  char buf[10];
  for (int i = 0; i < 10; i++) {
    buf[i] = '\0';
  }
  for (int i = 0 ; i < position; i++) {
    buf[i] = inData[i];
  }
  int moveData = atoi(buf);
  for (int i = 0; i < 10; i++) {
    buf[i] = '\0';
  }
  int idx = 0;
  for (int i = position + 1; i < strlen(inData); i++) {
    buf[idx] = inData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  if (moveData == 0 && rotateData == 0) {
    go(0,0);
  } else if (rotateData == 0) {
    if (moveData < 0) {
      go(-currentPower,-currentPower);
    } else {
      go(currentPower, currentPower);
    }
  } else {
    if (rotateData < 0) {
      go(-currentPower,currentPower);
    } else {
      go(currentPower, -currentPower);
    }
  }
  makeCleanup();
  isValidInput = true;
  return true;
}

bool moveOrRoatateWithDistanceCommand(char *buffer) {
  //remove m from command
  for (uint8_t i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  inData[strlen(inData)] = '\0';
  int position;
  for (uint8_t i = 0; i < strlen(inData); i++) {
    if (inData[i] == ',') {
      position = i;
      break;
    }
  }
  char buf[10];
  for (int i = 0; i < 10; i++) {
    buf[i] = '\0';
  }
  for (int i = 0 ; i < position; i++) {
    buf[i] = inData[i];
  }
  int moveData = atoi(buf);
  for (int i = 0; i < 10; i++) {
    buf[i] = '\0';
  }
  int idx = 0;
  for (int i = position + 1; i < strlen(inData); i++) {
    buf[idx] = inData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  if (moveData == 0 && rotateData == 0) {
    go(0,0);
  } else if (rotateData == 0) {
    moveLinear(moveData);
  } else {
    rotateDegree(rotateData);
  }
  makeCleanup();
  isValidInput = true;
  return true;
}

bool makeMove() {
  char buffer[SERIAL_BUFFER];
  for (int i = 0; i < SERIAL_BUFFER; i++) {
    buffer[i] = '\0';
  }
  if (index > 0) {
     inData[index-1] = '\0';
  }
  if (strlen(inData) == 1) {
    if (inData[0] == 'I') {
      sprintf(buffer,"unsupported\r\n");
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] == 'V') {      
      sprintf(buffer,"%d\r\n",maxPower);
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] =='v') {
      sprintf(buffer,"%d\r\n",minPower);
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] =='c') {
      sprintf(buffer,"%d\r\n",currentPower);
      Serial.print(buffer);
      Serial.flush();
    }  else if (inData[0] == 'd') {
      sprintf(buffer,"%d\r\n",0);
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] == 's') {
      sprintf(buffer,"%d\r\n",0);
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] == 'b') {
      sprintf(buffer,"OK\r\n");
      Serial.print(buffer);
      Serial.flush();
      breakAllEngines();
    } else if (inData[0] == 'C') {
      sprintf(buffer,"%d:%d:%d:%d\r\n",getLeftFrontEncoderCount(),getRightFrontEncoderCount(),getLeftBackEncoderCount(),getRightBackEncoderCount());
      Serial.print(buffer);
      Serial.flush();
    } else if (inData[0] == 'R') {
#ifdef SERIAL_DEBUG_MODE      
      sprintf(buffer,"%d:%d:%d:%d\r\n",getLeftFrontEncoderCount(),getRightFrontEncoderCount(),getLeftBackEncoderCount(),getRightBackEncoderCount());
#else
      sprintf(buffer,"%d\r\n",0);
#endif      
      resetCounters();      
      Serial.print(buffer);
      Serial.flush();
    } else {
      sprintf(buffer,"%d\r\n",0);
      Serial.print(buffer);
      Serial.flush();
      makeCleanup();
      isValidInput = false;
      return false;
    }
  } else {
      if (inData[0] == 'V') {
        return setMaxPowerCommand(buffer);
      } else if (inData[0] == 'v') {
        return setMinPowerCommand(buffer);
      } else if (inData[0] == 'c') {
        return setCurrentPowerCommand(buffer);
      } else if (inData[0] == 'd') {
        //this does not support low power distance
        return processPlatformUnsupoprtedCommand(buffer);
      } else if (inData[0] == 's') {
        //this does not support stop distance
        return processPlatformUnsupoprtedCommand(buffer);
      } else if (inData[0] == 'M') {
        return moveOrRotateUntilNextCommand(buffer);
      } else if (inData[0] == 'm') {
        return moveOrRoatateWithDistanceCommand(buffer);
      } else {
        sprintf(buffer,"%d\r\n",0);
        Serial.print(buffer);
        Serial.flush();
        makeCleanup();
        isValidInput = false;
        return false;
      }
    }
    isValidInput = true;
    makeCleanup();
    return true;
}

void loop()
{

  if (hasRFID) {
    if (isCardPresent()) {
      breakAllEngines();
      readRFID(inData);
      Serial.write(inData);
      makeMove();
      return;
    }
  }
  while(Serial.available() > 0) // Don't read unless there you know there is data
  {
     if(index < SERIAL_BUFFER - 1) // One less than the size of the array
     {
        inChar = Serial.read(); // Read a character
        inData[index] = inChar; // Store it
        index++; // Increment where to write next
        inData[index] = '\0'; // Null terminate the string
     } else {
        makeCleanup();
        cleanupSerial = true;
    }
  }
  if (inChar == '#') {
     makeMove();
     return;
  } else if (cleanupSerial) {
     makeCleanup();
     cleanupSerial = false;
  }
    delay(5);
}
