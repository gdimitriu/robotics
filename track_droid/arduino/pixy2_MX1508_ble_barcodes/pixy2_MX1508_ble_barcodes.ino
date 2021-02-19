/*
  Droid which recognize barcodes and move accordingly.
  Copyright 2019 Gabriel Dimitriu

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

#include <NeoSWSerial.h>
#include <PinChangeInt.h>
#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 9
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11

#define BLE_DEBUG_MODE true

#define MAX_POWER_ENGINE 255
#define TURN_90 1200 //1300 on carpet
#define MAX_CAMERA_BRIGHTNESS 255
bool autoCalibrationDone = false;
bool isStopped = false;
//for communication
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
boolean cleanupBT;

Pixy2I2C pixy;

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
  
  if (autoCalibrationDone) {
    return;
  }
  BTSerial.println("Autocalibration start ...");
  while(1) {
    res = pixy.line.getMainFeatures();
    //success get the barcode for forward move
    if (pixy.line.barcodes->m_code==2) {
      autoCalibrationDone = true;
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
        autoCalibrationDone = true;
        BTSerial.println("Autocalibration finished with no result keep full power led and maximum brightness ...");
        return;
      }
    }
    pixy.setCameraBrightness(newBrightness);
    delay(10);
  }
}
void makeMove() {
  if (index > 0) {
     inData[index-1] = '\0';
  }
  if (strcmp(inData,"s") == 0) {
    isStopped = !isStopped;
#ifdef BLE_DEBUG_MODE
    if (isStopped)
      BTSerial.println("stop");
    else
      BTSerial.println("start again");
#endif
    go(0,0);
  }
  makeCleanup();
}

void setup() 
{
  Serial.begin(38400);  
  BTSerial.begin(38400);
  
  Wire.begin();
  pixy.init();
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  attachPinChangeInterrupt(RxD, neoSSerial1ISR, CHANGE);
  BTSerial.println("Starting...");
  Serial.print("Starting...\n");
//  pixy.setServos(380,500); //this if for barcodes in front on posts
  pixy.setServos(380,0);
  go(0,0);  
  // change to the line_tracking program.  Note, changeProg can use partial strings, so for example,
  // you can change to the line_tracking program by calling changeProg("line") instead of the whole
  // string changeProg("line_tracking")
  pixy.changeProg("line");
  autoCalibrationDone = false;
  cleanupBT = false;
  // Turn off both laps upper and lower
  pixy.setLamp(0, 0);

}
int8_t droidDirection = 0;

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
  int8_t res;
  autocalibrationCamera();
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
    return;
 } else if (cleanupBT) {
    makeCleanup();
    cleanupBT = false;
 }
 if (isStopped)
    return;
  /*
   * 0 is stop
   * 1 is forward
   * 2 is bacward
   * 3 is left
   * 4 is right
   */
  // Get latest data from Pixy, including main vector, new intersections and new barcodes.
  res = pixy.line.getMainFeatures();

  if (res&LINE_BARCODE)
  {
    //pixy.line.barcodes->print();
    // code==0 is our left-turn sign
    if (pixy.line.barcodes->m_code==0)
    {
      BTSerial.println("left");
      go(-MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      delay(TURN_90);
      go(0,0);
      if (droidDirection == 1) {
        BTSerial.println("resuming forward");
        go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      } else if (droidDirection == 2) {
        BTSerial.println("resuming backward");
        go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      } else {
        BTSerial.println(droidDirection);
      }
    }
    // code==5 is our right-turn sign
    else if (pixy.line.barcodes->m_code==5)
    {
      BTSerial.println("right");
      go(MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      delay(TURN_90);
      go(0,0);
      if (droidDirection == 1) {
        BTSerial.println("resuming forward");
        go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      } else if (droidDirection == 2) {
        BTSerial.println("resuming backward");
        go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      } else {
        BTSerial.println(droidDirection);
      }
    }
    //code==1 is our stop sign for 10000 ms
    else if (pixy.line.barcodes->m_code==1)
    {
      BTSerial.println("stop");
      go(0,0);
      delay(10000);
    }
    //code==2 is our forward sign
    else if (pixy.line.barcodes->m_code==2)
    {
      Serial.println("forward");
      if (droidDirection != 1) {
        go(0,0);
      }
      droidDirection = 1;//forward
      go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
    }
    //code==3 is our backward sign
    else if (pixy.line.barcodes->m_code==3)
    {
      Serial.println("backward");
      if (droidDirection != 2) {
        go(0,0);
      }
      droidDirection = 2;//backward
      go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
    }
  }
}
