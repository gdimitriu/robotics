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

#include <Pixy2.h>

#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 5
#define LEFT_MOTOR_SPEED 4
#define RIGHT_MOTOR_PIN1 7

#define RIGHT_MOTOR_PIN2 8
#define RIGHT_MOTOR_SPEED 9
#define MAX_POWER_ENGINE 255
#define TURN_90 1900 //1300 on carpet
#define MAX_CAMERA_BRIGHTNESS 255
bool autoCalibrationDone = false;

Pixy2 pixy;
void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, LOW);
    return;
  }
  if (speedLeft > 0) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,HIGH);
    analogWrite(LEFT_MOTOR_SPEED, speedLeft);
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,HIGH);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, -speedLeft);
  }
 
  if (speedRight > 0) {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,HIGH);
    analogWrite(RIGHT_MOTOR_SPEED, speedRight);
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, -speedRight);
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
  Serial.println("Autocalibration start ...");
  while(1) {
    res = pixy.line.getMainFeatures();
    //success get the barcode for forward move
    if (pixy.line.barcodes->m_code==2) {
      autoCalibrationDone = true;
      Serial.println("Autocalibration finish with success ...");
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
        Serial.println("Autocalibration finished with no result keep full power led and maximum brightness ...");
        return;
      }
    }
    pixy.setCameraBrightness(newBrightness);
    delay(10);
  }
}

void setup() 
{
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  Serial.begin(115200);
  Serial.print("Starting...\n");
  go(0,0);
  pixy.init();
  // Turn on both lamps, upper and lower for maximum exposure
  pixy.setLamp(0, 0);
  // change to the line_tracking program.  Note, changeProg can use partial strings, so for example,
  // you can change to the line_tracking program by calling changeProg("line") instead of the whole
  // string changeProg("line_tracking")
  pixy.changeProg("line");
  autoCalibrationDone = false;
}
int8_t droidDirection = 0;

void loop()
{
  int8_t res;
  autocalibrationCamera();
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
    pixy.line.barcodes->print();
    // code==0 is our left-turn sign
    if (pixy.line.barcodes->m_code==0)
    {
      Serial.println("left");
      go(-MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      delay(TURN_90);
      go(0,0);
      if (droidDirection == 1) {
        Serial.println("resuming forward");
        go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      } else if (droidDirection == 2) {
        Serial.println("resuming backward");
        go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      } else {
        Serial.println(droidDirection);
      }
    }
    // code==5 is our right-turn sign
    else if (pixy.line.barcodes->m_code==5)
    {
      Serial.println("right");
      go(MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      delay(TURN_90);
      go(0,0);
      if (droidDirection == 1) {
        Serial.println("resuming forward");
        go(MAX_POWER_ENGINE,MAX_POWER_ENGINE);
      } else if (droidDirection == 2) {
        Serial.println("resuming backward");
        go(-MAX_POWER_ENGINE,-MAX_POWER_ENGINE);
      } else {
        Serial.println(droidDirection);
      }
    }
    //code==1 is our stop sign for 10000 ms
    else if (pixy.line.barcodes->m_code==1)
    {
      Serial.println("stop");
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
