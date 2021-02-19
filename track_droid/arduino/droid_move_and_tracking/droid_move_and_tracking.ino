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
#include <PinChangeInt.h>

#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 9
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11

#define BLE_DEBUG_MODE true

#define TURN_90 1200
#define MAX_CAMERA_BRIGHTNESS 255
#define MAX_POWER_ENGINE 255

//for communication
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
boolean cleanupBT;

bool isStopped = false;
bool isTracking = false;
bool isLampOn = false;
bool isProgramChanged = true;
bool isAutoMode = false;
int tempIValue;
int8_t droidDirection = 0;
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
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedRight);
#endif
  }
}

/*
 * Autocalibration of the brightness and led based on the environment light to detect barcodes.
 */
void autocalibrationCamera()
{
  int8_t res;
  int8_t newBrightness = 0;
  bool lowerLed = false;
  bool upperLed = false;
  bool lastTry = false;
  
  BTSerial.println("Autocalibration start ...");
  while(1) {
    res = pixy.line.getMainFeatures();
    //success get the barcode for forward move
    if (pixy.line.barcodes->m_code==2) {
      BTSerial.println("Autocalibration finish with success ...");
      return;
    } else if (newBrightness < MAX_CAMERA_BRIGHTNESS) {
      newBrightness +=5;
    } 
    if (newBrightness >= MAX_CAMERA_BRIGHTNESS) {
      newBrightness = MAX_CAMERA_BRIGHTNESS;
      if (!lowerLed) {
        pixy.setLamp(0,1);
        lowerLed = true;
      } else if (lowerLed && !upperLed) {
        pixy.setLamp(1,1);
        upperLed = true;        
      } else if (upperLed && !lastTry) {
        newBrightness = 0;
        lastTry = true;
      } else {
        BTSerial.println("Autocalibration finished with no result keep full power led and maximum brightness ...");
        return;
      }
    }
    pixy.setCameraBrightness(newBrightness);
    delay(10);
  }
}

void printMenuOnBLE() {
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "Moving and tracking droid\n");
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "MENU:\n" );
  BTSerial.println( "h# print this menu\n");
  BTSerial.println( "S# Start\n" );
  BTSerial.println( "s# Stop\n" );
  BTSerial.println( "T# tracking on\n");
  BTSerial.println( "t# tracking off\n");
  BTSerial.println( "L# lamp on/off\n");
  BTSerial.println( "A# autocalibration of pixy\n");
  BTSerial.println( "a# auto mode\n");
  BTSerial.println( "pxx# position of camera -1 down 0 front\n");
  BTSerial.println( "fxxx# move forward with xxx power util stop\n");
  BTSerial.println( "bxxx# move backward with xxx power until stop\n");
  BTSerial.println( "lxxx# rotate left with xxx power until stop\n");
  BTSerial.println( "rxxx# rotate right with xxx power until stop\n");
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
    panLoop.reset();
    tiltLoop.reset();
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);  
  } else if(strcmp(inData,"T") == 0) {
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Tracking on");
#endif
    panLoop.reset();
    tiltLoop.reset();
    pixy.setServos(panLoop.m_command, tiltLoop.m_command);  
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
  } else if (strcmp(inData,"A") == 0) {
    isStopped = true;
    isTracking = false;
    go(0,0);
    // Use color connected components program for the pan tilt to track 
    pixy.changeProg("line");    
    isProgramChanged = true;
    autocalibrationCamera();
  } else if (strcmp(inData,"a") == 0) {
    isAutoMode = true;
    isStopped = false;
    isTracking = false;
    droidDirection = 0;
    // Use color connected components program for the pan tilt to track 
    pixy.changeProg("line");
    isProgramChanged = true;
  } else if (strlen(inData) > 1) {
      if (inData[0] == 'f') {
        //remove f from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          makeCleanup();
          return;
        }
        tempIValue = atoi(inData);
        go(tempIValue,tempIValue);
      } else  if (inData[0] == 'b') {
        //remove b from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          makeCleanup();
          return;
        }
        tempIValue = atoi(inData);
        go(-tempIValue,-tempIValue);
      } else  if (inData[0] == 'l') {
        //remove l from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          makeCleanup();
          return;
        }
        tempIValue = atoi(inData);
        go(-tempIValue,tempIValue);
      } else  if (inData[0] == 'r') {
        //remove r from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          makeCleanup();
          return;
        }
        tempIValue = atoi(inData);
        go(tempIValue,-tempIValue);
      } else if (inData[0] == 'p') {
        //remove p from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          makeCleanup();
          return;
        }
        tempIValue = atoi(inData);
        switch(tempIValue){
          case -1:
            //down camera barcode are on the ground
            pixy.setServos(380,0);
            break;
          case 0:
            //front camera barcodes in front on posts
            pixy.setServos(380,500); 
            break;
        } 
      }
    }
    makeCleanup();
}

void setup() 
{
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
  printMenuOnBLE();
  isTracking = false;
  isLampOn = false;
  // Turn off both laps upper and lower
  pixy.setLamp(0, 0);
  isProgramChanged = true;
  isAutoMode = false;
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
    if (isProgramChanged) {
      // Use color connected components program for the pan tilt to track 
      pixy.changeProg("color_connected_components");
      isProgramChanged = false;
    }
  
    // get active blocks from Pixy
    pixy.ccc.getBlocks();
  
    if (pixy.ccc.numBlocks) {        
      // calculate pan and tilt "errors" with respect to first object (blocks[0]), 
      // which is the biggest object (they are sorted by size).  
      panOffset = (int32_t)pixy.frameWidth/2 - (int32_t)pixy.ccc.blocks[0].m_x;
      tiltOffset = (int32_t)pixy.ccc.blocks[0].m_y - (int32_t)pixy.frameHeight/2;  
  
      // update loops
      panLoop.update(panOffset);
      tiltLoop.update(tiltOffset);
  
      // set pan and tilt servos  
      pixy.setServos(panLoop.m_command, tiltLoop.m_command);
    } else // no object detected, go into reset state
    {
      panLoop.reset();
      tiltLoop.reset();
      pixy.setServos(panLoop.m_command, tiltLoop.m_command);
    }
  }
  if (isAutoMode) {
    int8_t res;
    /*
    * 0 is stop
    * 1 is forward
    * 2 is bacward
    * 3 is left
    * 4 is right
    */
    // Get latest data from Pixy, including main vector, new intersections and new barcodes.
    res = pixy.line.getMainFeatures();

    if  (res&LINE_BARCODE)
    {
      // code==0 is our left-turn sign
      if (pixy.line.barcodes->m_code==0)
      {
#if BLE_DEBUG_MODE
        BTSerial.println("left");
#endif      
        go(-MAX_POWER_ENGINE,MAX_POWER_ENGINE);
        delay(TURN_90);
        go(0,0);
        if (droidDirection == 1) {
#if BLE_DEBUG_MODE
          BTSerial.println("resuming forward");
#endif        
          go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
        } else if (droidDirection == 2) {
#if BLE_DEBUG_MODE        
          BTSerial.println("resuming backward");
#endif        
          go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
        } else {
#if BLE_DEBUG_MODE        
          BTSerial.println(droidDirection);
#endif
        }
    }
    // code==5 is our right-turn sign
    else if (pixy.line.barcodes->m_code==5)
    {
#if BLE_DEBUG_MODE      
      BTSerial.println("right");
#endif
      go(MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      delay(TURN_90);
      go(0,0);
      if (droidDirection == 1) {
#if BLE_DEBUG_MODE
        BTSerial.println("resuming forward");
#endif
        go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      } else if (droidDirection == 2) {
#if BLE_DEBUG_MODE
        BTSerial.println("resuming backward");
#endif
        go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      } else {
#if BLE_DEBUG_MODE
        BTSerial.println(droidDirection);
#endif
      }
    }
    //code==1 is our stop sign for 10000 ms
    else if (pixy.line.barcodes->m_code==1)
    {
#if BLE_DEBUG_MODE
      BTSerial.println("stop");
#endif
      go(0,0);
      delay(10000);
    }
    //code==2 is our forward sign
    else if (pixy.line.barcodes->m_code==2)
    {
#if BLE_DEBUG_MODE
      Serial.println("forward");
#endif
      if (droidDirection != 1) {
        go(0,0);
      }
      droidDirection = 1;//forward
      go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
    }
    //code==3 is our backward sign
    else if (pixy.line.barcodes->m_code==3)
    {
#if BLE_DEBUG_MODE
      Serial.println("backward");
#endif
      if (droidDirection != 2) {
        go(0,0);
      }
      droidDirection = 2;//backward
      go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
    }
  }
  }
}
