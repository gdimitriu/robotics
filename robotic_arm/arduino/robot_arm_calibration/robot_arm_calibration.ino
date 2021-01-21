/*
 * robot arm calibration.
 * Copyright 2019 Gabriel Dimitriu
 *
 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/

#include<Arduino.h>
#include<Servo.h>
#define servoPin_Gripper 12
#define servoPin_WristRoll 11
#define servoPin_WristPitch 10
#define servoPin_Elbow 9
#define servoPin_Shoulder 8
#define servoPin_Waist 7

Servo servo_Gripper;
Servo servo_WristRoll;
Servo servo_WristPitch;
Servo servo_Elbow;
Servo servo_Shoulder;
Servo servo_Waist;

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
  Serial.begin(9600);
  servo_Gripper.attach(servoPin_Gripper);
  servo_WristRoll.attach(servoPin_WristRoll);
  servo_WristPitch.attach(servoPin_WristPitch);
  servo_Elbow.attach(servoPin_Elbow);
  servo_Shoulder.attach(servoPin_Shoulder);
  servo_Waist.attach(servoPin_Waist);
  Serial.println("after init");
}

void loop() {
   serialCallibration();
}

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
        servo_Waist.write(atoi(inData));
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
        servo_Shoulder.write(180-atoi(inData));
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
        servo_Elbow.write(atoi(inData));
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
        servo_WristRoll.write(atoi(inData));
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
        servo_WristPitch.write(180-atoi(inData));
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
        servo_Gripper.write(atoi(inData));
        isValidInput = true;
    } else {
      isValidInput = false;
    }
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
