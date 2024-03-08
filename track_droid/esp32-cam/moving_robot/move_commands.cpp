#include "move_commands.h"

int currentPower = 0;
int minPower = 255;
int maxPower = 4095;

bool isValidInput = false;

static const int engineLeft1Channel = 0;
static const int engineLeft2Channel = 1;
static const int engineRight1Channel = 2;
static const int engineRight2Channel = 3;
static const int freq = 50;
static const int resolution = 12;

bool isValidNumber( char *data, int size )
{
  if ( size == 0 ) return false;
   bool isNum=false;
   if( !( data[0] == '+' || data[0] == '-' || isDigit(data[0]) ) ) return false;

   for( byte i=1; i<size; i++ )
   {
       if( !( isDigit(data[i]) || data[i] == '.' ) ) return false;
   }
   return true;
}

bool moveOrRoatateWithDistanceCommand(char *realData) {
  //remove m from command
  realData++;
  realData[strlen(realData)] = '\0';
  int position;
  for ( uint8_t i = 0; i < strlen(realData); i++ ) {
    if ( realData[i] == ',' ) {
      position = i;
      break;
    }
  }
  char buf[10];
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  for ( int i = 0 ; i < position; i++ ) {
    buf[i] = realData[i];
  }
  int moveData = atoi(buf);
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  int idx = 0;
  for ( int i = position + 1; i < strlen(realData); i++ ) {
    buf[idx] = realData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  if ( moveData == 0 && rotateData == 0 ) {
    Serial.println("coasting");
  } else if ( rotateData == 0 ) {
    Serial.print("move=");Serial.println(rotateData);
  } else {
    Serial.print("rotate=");Serial.println(rotateData);
  }
  makeCleanup();
  isValidInput = true;
  return true;
}


bool setMaxPowerCommand(char *realData)
{
  sendOK();
  //remove V from command
  realData++;
  if ( !isValidNumber(realData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(realData) > ABSOLUTE_MAX_POWER || atol(realData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  maxPower = atol(realData);
  makeCleanup();
  isValidInput = true;
  return true;
}

bool setMinPowerCommand(char *realData)
{
  sendOK();
  //remove v from command
  realData++;
  if ( !isValidNumber(realData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(realData) > maxPower || atol(realData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  minPower = atol(realData);
  makeCleanup();
  isValidInput = true;
  return true;
}


bool setCurrentPowerCommand(char *realData)
{
  sendOK();
  //remove c from command
  realData++;
  if ( !isValidNumber(realData, indexReceive - 2) ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  if ( atol(realData) > maxPower || atol(realData) < 0 ) {
    isValidInput = false;
    makeCleanup();
    return false;
  }
  currentPower = atol(realData);
  makeCleanup();
  isValidInput = true;
  return true;
}


bool moveOrRotateUntilNextCommand(char *realData)
{
  //remove M from command
  realData++;
  realData[strlen(realData)] = '\0';
  int position;
  for ( uint8_t i = 0; i < strlen(realData); i++ ) {
    if ( realData[i] == ',' ) {
      position = i;
      break;
    }
  }
  char buf[10];
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  for ( int i = 0 ; i < position; i++ ) {
    buf[i] = realData[i];
  }
  int moveData = atoi(buf);
  for ( int i = 0; i < 10; i++ ) {
    buf[i] = '\0';
  }
  int idx = 0;
  for ( int i = position + 1; i < strlen(realData); i++ ) {
    buf[idx] = realData[i];
    idx++;
  }
  int rotateData = atoi(buf);
  Serial.print("move=");Serial.print(moveData);Serial.print(" rotate=");Serial.println(rotateData);
  if ( moveData == 0 && rotateData == 0 ) {
    go(0,0);
  } else if ( rotateData == 0 ) {
    if ( moveData < 0 ) {
       go(-currentPower,-currentPower);
    } else {
       go(currentPower,currentPower);
    }
  } else {
     if ( rotateData < 0 ) {
      go(-currentPower, currentPower);
    } else {
      go(currentPower, -currentPower);
    }
  }
  makeCleanup();
  isValidInput = true;
  return true;
}

void setupEngines()
{
  // configure right engine PWM functionalitites
  ledcSetup(engineRight1Channel, freq, resolution);
  ledcSetup(engineRight2Channel, freq, resolution);
  // attach the channel to the GPI to be controlled
  ledcAttachPin(RIGHT_MOTOR_PIN1, engineRight1Channel);
  ledcAttachPin(RIGHT_MOTOR_PIN2, engineRight2Channel);
  // configure left engine PWM functionalitites
  ledcSetup(engineLeft1Channel, freq, resolution);
  ledcSetup(engineLeft2Channel, freq, resolution);
  // attach the channel to the GPI to be controlled
  ledcAttachPin(LEFT_MOTOR_PIN1, engineLeft1Channel);
  ledcAttachPin(LEFT_MOTOR_PIN2, engineLeft2Channel);
}

/*
* Move the platform in predefined directions.
*/
void go( int speedLeft, int speedRight )
{

  if ( speedLeft == 0 && speedRight == 0 ) {
    ledcWrite(engineLeft1Channel, 0);
    ledcWrite(engineLeft2Channel, 0);
    ledcWrite(engineRight1Channel, 0);
    ledcWrite(engineRight2Channel, 0);
    return;
  }
  if ( speedLeft > 0 ) {
    ledcWrite(engineLeft1Channel, speedLeft);
    ledcWrite(engineLeft2Channel, 0);
  } 
  else {
    ledcWrite(engineLeft1Channel, 0);
    ledcWrite(engineLeft2Channel, -speedLeft);    
  }
 
  if ( speedRight > 0 ) {
    ledcWrite(engineRight1Channel, speedRight);
    ledcWrite(engineRight2Channel, 0);
  }else {
    ledcWrite(engineRight1Channel, 0);
    ledcWrite(engineRight2Channel, -speedRight);
  }
}
