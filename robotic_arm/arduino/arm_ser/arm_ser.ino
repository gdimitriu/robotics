/*
 * arm with serial
 * Copyright 2022 Gabriel Dimitriu
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

#define servoPin_Gripper 6
#define servoPin_Elbow 5
#define servoPin_Shoulder 4
#define DELAY 100
Servo servo_Gripper;
Servo servo_Elbow;
Servo servo_Shoulder;
//for communication
char inDataBT[20]; // Allocate some space for the string
char inDataSerial[20]; // Allocate some space for the string
char inCharSerial; // Where to store the character read
byte indexSerial = 0; // Index into array; where to store the character
boolean cleanupSerial;
int currentShoulderPos = 0;
int currentElbowPos = 180;

boolean isValidNumber(char *data, int size) {
  if (size == 0 ) return false;
   boolean isNum=false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++) {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

void printMenu() {
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "Calibration of robot arm");
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "sxx# servo shoulder x degree");
  Serial.println( "exx# servo elbow x degree");
  Serial.println( "gxx# gripper x degree");
  Serial.println( "fxx# follow x degree");
  Serial.println( "-----------------------------" );
}

void setup() {
  Serial.begin(9600);
  cleanupSerial = false;
  servo_Gripper.attach(servoPin_Gripper);
  servo_Elbow.attach(servoPin_Elbow);
  servo_Shoulder.attach(servoPin_Shoulder);
  Serial.println("after init");
  servo_Elbow.write(currentElbowPos);
  servo_Shoulder.write(currentShoulderPos);
  printMenu();
}

void cleanup(bool isBT) {
   for (indexSerial = 0; indexSerial < 20; indexSerial++) {
      inDataSerial[indexSerial] = '\0';
   }
   inCharSerial = '0';
   indexSerial = 0;
   cleanupSerial = false;
}

bool makeMove(char *inData,int index, bool isBT) {
  if (inData[0] == 's') {
     //remove s from command
     for (int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
        cleanup(isBT);
        return false;
     }
     int target = atoi(inData);
     if (target < 0 || target > 180) {
        Serial.print("Value out of range");
        cleanup(isBT);
        return false;
     }
     Serial.print("move servo shoulder ");
     Serial.print(target);
     Serial.println("degree");
     if ( currentShoulderPos < target) {     
          for (; currentShoulderPos <= target; currentShoulderPos++)  {
              servo_Shoulder.write(currentShoulderPos);
              delay(DELAY);
          }
     } else {
        for (; currentShoulderPos >= target; currentShoulderPos--) {
            servo_Shoulder.write(currentShoulderPos);
            delay(DELAY);
        }
     }
     cleanup(isBT);
     return true;
  } else if (inData[0] == 'e') {
     //remove e from command
     for (int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
        cleanup(isBT);
        return false;
     }
     int target = atoi(inData);
     if (target < 0 || target > 180) {
        Serial.print("Value out of range");
        cleanup(isBT);
        return false;
     }
     Serial.print("move servo elbow ");
     Serial.print(target);
     Serial.println("degree");
     if ( currentElbowPos < target) {     
         for (; currentElbowPos <= target; currentElbowPos++) {
             servo_Elbow.write(currentElbowPos);
             delay(DELAY);
         }
     } else {
         for (; currentElbowPos >= target; currentElbowPos--) {
             servo_Elbow.write(currentElbowPos);
             delay(DELAY);
         }
     }
     cleanup(isBT);
     return true;
  } else if (inData[0] == 'f') {
     //remove f from command
     for (int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
        cleanup(isBT);
        return false;
     }
     int target = atoi(inData);
     if (target < 0 || target > 180) {
        Serial.print("Value out of range");
        cleanup(isBT);
        return false;
     }
     Serial.print("move follow ");
     Serial.print(target);
     Serial.println("degree");
     if ( currentShoulderPos < target) {
         for (; currentShoulderPos <= target; currentElbowPos-- , currentShoulderPos++) {
             servo_Shoulder.write(currentShoulderPos);
             servo_Elbow.write(currentElbowPos);
             delay(DELAY);
         }
     } else {
         for (; currentShoulderPos >= target; currentElbowPos++ , currentShoulderPos-- ) {
             servo_Shoulder.write(currentShoulderPos);
             servo_Elbow.write(currentElbowPos);
             delay(DELAY);
         }
     }
     cleanup(isBT);
     return true;
  } else if (inData[0] == 'g') {
     //remove g from command
     for (int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
       cleanup(isBT);
       return false;
     }
     Serial.print("move servo grabber");
     Serial.print(atoi(inData));
     Serial.println("degree");
     servo_Gripper.write(atoi(inData));
     cleanup(isBT);
     return true;
  }
  cleanup(isBT);
  return false;
}

void readData() {
  
   while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
   {
      if(indexSerial < 19) // One less than the size of the array
      {
         inCharSerial = Serial.read(); // Read a character
         if (inCharSerial == '\r' || inCharSerial == '\n' || inCharSerial == '\0' || inCharSerial < 35 || inCharSerial > 122) {
            continue;
         }
         //commands start with a letter capital or small
         if (indexSerial == 0 && !((inCharSerial > 64 && inCharSerial < 91) || (inCharSerial > 96 && inCharSerial < 123))) {
            continue;
         }
         inDataSerial[indexSerial++] = inCharSerial; // Store it
         inDataSerial[indexSerial] = '\0'; // Null terminate the string
      } else {
        cleanup(false);
      }
   }
  
   if (inDataSerial[indexSerial-1] == '#') {
      if (indexSerial > 0) {
         inDataSerial[indexSerial-1] = '\0';
      }
      if (!makeMove(inDataSerial,indexSerial, false)) {        
         printMenu();
      }
   }
}

void loop() {
   while( !Serial.available())
        ; // LOOP...
   readData();
}
