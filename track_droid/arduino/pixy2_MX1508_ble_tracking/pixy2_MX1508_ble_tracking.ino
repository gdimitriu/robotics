/*
  Droid with tracking turret using pixy2
  Copyright 2021 Gabriel Dimitriu

  This file is part of Robotics

  Robotics is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  Robotics is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with Robotics; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/

#include <Pixy2I2C.h>
#include <PIDLoop.h>
#include <Wire.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 9
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11

#define MIN_ENGINE_POWER 150
#define MAX_ENGINE_POWER 255
#define BLE_DEBUG_MODE true
#define SERIAL_DEBUG_MODE true

#define TURN_90 1200

//for communication
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
boolean cleanupBT;

bool isStopped = true;
bool isTracking = false;
bool isLampOn = false;
bool isEngineTracking = false;
int32_t tempIValue;
float Kp = -0.25;
float Kd = 100;
float Ki = 0;
float tempDValue;
int32_t lastError = 0;
int32_t integration = 0;

int32_t panServoPos = 380;

Pixy2I2C pixy;
PIDLoop panLoop(-300, 0, -5, true);
PIDLoop tiltLoop(-400, 0, -5, true);
NeoSWSerial BTSerial(RxD, TxD);

void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE    
    if (Serial) {
      Serial.println("All on zero");
    }
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    if (Serial) {
      Serial.print("Left "); Serial.print(speedLeft); Serial.print(" , "); Serial.println(0);
    }
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
    if (Serial) {
      Serial.print("Left "); Serial.print(0); Serial.print(" , "); Serial.println(-speedLeft);
    }
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    if (Serial) {
      Serial.print("Right "); Serial.print(speedRight); Serial.print(" , "); Serial.println(0);
    }
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef SERIAL_DEBUG_MODE
    if (Serial) {
      Serial.print("Right "); Serial.print(0); Serial.print(" , "); Serial.println(-speedRight);
    }
#endif
  }
}

void printMenuOnBLE() {
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "Tracking using driver MX1508\n");
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "MENU:\n" );
  BTSerial.println( "h# print this menu\n");
  BTSerial.println( "S# Start\n" );
  BTSerial.println( "s# Stop\n" );
  BTSerial.println( "T# tracking on\n");
  BTSerial.println( "t# tracking off\n");
  BTSerial.println( "L# lamp on/off\n");
  BTSerial.println( "E# engine pan tracking on/off\n");
  BTSerial.println( "pxx# horizontal position of camera\n");
  BTSerial.println( "Pxxx# Kp\n");
  BTSerial.println( "Ixxx# Ki\n");
  BTSerial.println( "Dxxx# Kd\n");
  BTSerial.println( "-----------------------------\n" );
}

boolean isValidNumber(char *data, int size)
{
  if (size == 0 ) return false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++)
   {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

void makeMove() {
  if (index > 0) {
     inData[index-1] = '\0';
  }
  if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE
    BTSerial.println("stop");
#endif
    go(0,0);
    isStopped = true;
  } else if (strcmp(inData,"S") == 0) {
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Start");
#endif
    go(0,0);
    isStopped = false;
  } else if(strcmp(inData,"t") == 0) {
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Tracking off");
#endif
    isTracking = false;
  } else if(strcmp(inData,"T") == 0) {
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Tracking on");
#endif
    lastError = 0;
    integration = 0;
    isTracking = true;
  } else if (strcmp(inData,"h") == 0) {
    printMenuOnBLE();
  } else if (strcmp(inData,"L") == 0) {
    if (isLampOn) {
      // Turn off both laps upper and lower
      pixy.setLamp(0, 0);
    } else {
      pixy.setLamp(1,1);
    }
    isLampOn = !isLampOn;
  } else if (strcmp(inData,"E") == 0) {
    isEngineTracking = !isEngineTracking;
#ifdef BLE_DEBUG_MODE
    if (isEngineTracking)
      BTSerial.println("Engine tracking on");
    else
      BTSerial.println("Engine tracking off");
#endif
  }else if (strlen(inData) > 1) {
    if (inData[0] == 'p') {
      //remove p from command
      for (unsigned int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
      }
      if (!isValidNumber(inData, index - 2)) {
        makeCleanup();
        return;
      }
      tempIValue = atoi(inData);
      if (tempIValue <= 1000 && tempIValue >=0) {
          pixy.setServos(tempIValue,500);
          panServoPos = tempIValue;
      } else {
        makeCleanup();
        return;
      }
    } else if (inData[0] == 'P') {
      //remove P from command
      for (unsigned int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
      }
      tempDValue = atof(inData);
#ifdef BLE_DEBUG_MODE
      BTSerial.print("Kp=");BTSerial.print(Kp);BTSerial.print("->");BTSerial.println(tempDValue);
#endif
      Kp=tempDValue;
      lastError = 0;
      integration = 0;
      tiltLoop.reset();
      pixy.setServos(panServoPos, tiltLoop.m_command);
    } else if (inData[0] == 'D') {
      //remove D from command
      for (unsigned int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
      }
      tempDValue = atof(inData);
#ifdef BLE_DEBUG_MODE
      BTSerial.print("Kd=");BTSerial.print(Kd);BTSerial.print("->");BTSerial.println(tempDValue);
#endif
      Kd=tempDValue;
      lastError = 0.;
      integration = 0;
      tiltLoop.reset();
      pixy.setServos(panServoPos, tiltLoop.m_command);
    } else if (inData[0] == 'I') {
      //remove I from command
      for (unsigned int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
      }
      tempDValue = atof(inData);
#ifdef BLE_DEBUG_MODE
      BTSerial.print("Ki=");BTSerial.print(Ki);BTSerial.print("->");BTSerial.println(tempDValue);
#endif
      Ki=tempDValue;
      lastError = 0;
      integration = 0;
      tiltLoop.reset();
      pixy.setServos(panServoPos, tiltLoop.m_command);
    }
  }
  makeCleanup();
}

void setup() 
{
#ifdef SERIAL_DEBUG_MODE
  Serial.begin(38400);
  Serial.println("Starting...");
#endif
  BTSerial.begin(38400);  
  Wire.begin();
  pixy.init();
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  attachPinChangeInterrupt(RxD, neoSSerial1ISR, CHANGE); 
  BTSerial.println("Starting...");
  cleanupBT = false;
#ifdef BLE_DEBUG_MODE
  printMenuOnBLE();
#endif
  isTracking = false;
  isLampOn = false;
  isStopped = true;
  // Turn off both laps upper and lower
  pixy.setLamp(0, 0);
  pixy.changeProg("color_connected_components");
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void loop()
{
  while(BTSerial.available() > 0) // Don't read unless there you know there is data
  {
     if(index < 19) // One less than the size of the array
     {
        inChar = BTSerial.read(); // Read a character
        inData[index++] = inChar; // Store it
        inData[index] = '\0'; // Null terminate the string
     } else {
        makeCleanup();
        cleanupBT = true;
     }
 }
 if (inChar == '#') {
    makeMove();
 } else if (cleanupBT) {
    makeCleanup();
    cleanupBT = false;
 }
 
 if (isStopped)
    return;

 if (isTracking) {
  int32_t panOffset, tiltOffset;
  int32_t left, right;
  // Use color connected components program for the pan tilt to track 
  pixy.changeProg("color_connected_components");
  
  // get active blocks from Pixy
  pixy.ccc.getBlocks();
  
  if (pixy.ccc.numBlocks) {        
    // calculate pan and tilt "errors" with respect to first object (blocks[0]), 
    // which is the biggest object (they are sorted by size).  
    panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)pixy.ccc.blocks[0].m_x;
    tiltOffset = (int32_t)pixy.ccc.blocks[0].m_y - (int32_t)pixy.frameHeight/2;  
  
    // update loops    
    tiltLoop.update(tiltOffset);
    if (isEngineTracking) {
      double command;
      int32_t P,D;
      integration += panOffset;
      P = panOffset;
      if (integration > 255)
        integration = 255;
      if (integration < -255)
        integration = -255;
      D = panOffset - lastError;
      if (D > 255)
        D = 255;
      if (D < -255)
        D = -255;
      lastError = panOffset;
      command = Kp*P+Ki*integration+Kd*D;
      if (panOffset < 0) {
        left = command;
        if (left < MIN_ENGINE_POWER)
          left = MIN_ENGINE_POWER;
        right = -command;
        if (right > -MIN_ENGINE_POWER)
          right = - MIN_ENGINE_POWER;      
      } else {
        left = - command;
        if (left > - MIN_ENGINE_POWER)
          left = - MIN_ENGINE_POWER;
        right = command;
        if (right < MIN_ENGINE_POWER)
          right = MIN_ENGINE_POWER;      
      }
      pixy.setServos(panServoPos, tiltLoop.m_command);
      if (left < -MAX_ENGINE_POWER)
        left = -MAX_ENGINE_POWER;
      else if (left > MAX_ENGINE_POWER)
        left = MAX_ENGINE_POWER;
      if (right < -MAX_ENGINE_POWER)
        right = -MAX_ENGINE_POWER;
      else if (right > MAX_ENGINE_POWER)
        right = MAX_ENGINE_POWER;
      go(left,right);
    } else {
      panLoop.update(panOffset);
      // set pan and tilt servos  
      pixy.setServos(panLoop.m_command, tiltLoop.m_command);
    }
  } else // no object detected, go into reset state
   {
    go(0,0);
    panLoop.reset();
    tiltLoop.reset();
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);
  }  
 } else {
    panLoop.reset();
    tiltLoop.reset();
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);
  }
  delay(10);
}
