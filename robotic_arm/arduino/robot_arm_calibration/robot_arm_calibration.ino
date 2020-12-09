/*
robot arm calibration.
Copyright 2019 Gabriel Dimitriu

This file is part of Robotic Calibrations

Robotic Calibrations is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Robotic Calibrations is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Robotic Calibrations; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
/**
 * this has to be done to not break arms.
 * https://stackoverflow.com/questions/26695670/attach-arduino-servo-without-moving
 */
#include<Arduino.h>
#include<Servo.h>
/*
#include<Wire.h>
#include<PCF8574.h>
#include<VL53L0X.h>
*/
#define servoPin_Gripper 11
#define servoPin_WristRoll 10
#define servoPin_WristPitch 9
#define servoPin_Elbow 6
#define servoPin_Shoulder 5
#define servoPin_Waist 4

#define DEBUG 0

Servo servo_Gripper;
Servo servo_WristRoll;
Servo servo_WristPitch;
Servo servo_Elbow;
Servo servo_Shoulder;
Servo servo_Waist;
//VL53L0X sensorDistance;
int servo_GripperOldPosition = 90;
int servo_WristRollOldPosition = 90;
int servo_WristPitchOldPosition = 90;
int servo_ElbowOldPosition = 90;
int servo_ShoulderOldPosition = 90;
int servo_WaistOldPosition = 90;
int delayMs = 0;

void slowMoveServoToPosition(int *oldPosition, int newPosition, Servo *servo, int delayMs) {
  /*
   if(*oldPosition<newPosition) {
    for (int i = *oldPosition; i <= newPosition; i++) {
      servo->write(i);
       if (delayMs > 0) {
        delay(delayMs);
      }
    }
  } else {
    for (int i = *oldPosition; i >= newPosition; i--) {
      servo->write(i);
       if (delayMs > 0) {
        delay(delayMs);
      }
    }
  }
  *oldPosition = newPosition;
  */
  servo->write(newPosition);
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

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
  #endif
  servo_Gripper.attach(servoPin_Gripper);
  servo_WristRoll.attach(servoPin_WristRoll);
  servo_WristPitch.attach(servoPin_WristPitch);
  servo_Elbow.attach(servoPin_Elbow);
  servo_Shoulder.attach(servoPin_Shoulder);
  servo_Waist.attach(servoPin_Waist);
//  Wire.begin();
//  sensorDistance.init();
  #ifdef DEBUG
    Serial.println("after init");
  #endif
//  sensorDistance.startContinuous();
}

void loop() {
  // put your main code here, to run repeatedly:
  #ifdef DEBUG
    serialCallibration();
  #endif
}
#ifdef DEBUG
  bool isValidInput;
  char inData[20]; // Allocate some space for the string
  char inChar; // Where to store the character read
  byte index = 0; // Index into array; where to store the character

void serialCallibration() {
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "Calibration of robot arm");
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "wxx# servo waist x degree");
  Serial.println( "sxx# servo shoulder x degree");
  Serial.println( "exx# servo elbow x degree");
  Serial.println( "pxx# servo wrist pitch x degre");
  Serial.println( "rxx# servo wrist roll x degree");
  Serial.println( "gxx# gripper x degree");
  Serial.println( "dxx# set the delay to ms between moving 1 degree");
  Serial.println( "-----------------------------" );
   do {
    for (index = 0; index < 20; index++)
    {
       inData[index] = '\0';
    }
    inChar = '0';
    index = 0;
    while(inChar != '#') {
      while( !Serial.available() )
        ; // LOOP...
      while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = Serial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
              inData[index] = '\0'; // Null terminate the string
          }
      }
    }
    if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strlen(inData) > 1) {
      if (inData[0] == 'w') {
        //remove w from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo waist ");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_WaistOldPosition, atoi(inData), &servo_Waist, delayMs);
        isValidInput = true;
      } else if (inData[0] == 's') {
        //remove s from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo shoulder ");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_ShoulderOldPosition, 180-atoi(inData), &servo_Shoulder, delayMs);
        isValidInput = true;
      } else if (inData[0] == 'e') {
        //remove e from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo elbow ");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_ElbowOldPosition, atoi(inData), &servo_Elbow, delayMs);
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo wrist roll ");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_WristRollOldPosition, atoi(inData), &servo_WristRoll, delayMs);
        isValidInput = true;
      } else if (inData[0] == 'p') {
        //remove p from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo wrist pitch ");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_WristPitchOldPosition, 180- atoi(inData), &servo_WristPitch, delayMs);
        isValidInput = true;
      } else if (inData[0] == 'g') {
        //remove g from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("move servo grabber");
        Serial.print(atoi(inData));
        Serial.println("degree");
        slowMoveServoToPosition(&servo_GripperOldPosition, atoi(inData), &servo_Gripper, delayMs);
        isValidInput = true;
    } else if (inData[0] == 'd') {
        //remove d from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          Serial.println("Is not a number");
          Serial.println(inData);
          break;
        }
        Serial.print("set dalay in ms for moving ");
        Serial.print(atoi(inData));
        Serial.println("ms");
        delayMs = atoi(inData);
        isValidInput = true;
    }else {
      isValidInput = false;
    }
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}

#endif
