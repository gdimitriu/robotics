/*
 * mini robot arm
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
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 3
#define servoPin_Gripper 4
#define servoPin_Elbow 5
#define servoPin_Shoulder 6
#define servoPin_Waist 7

Servo servo_Gripper;
Servo servo_Elbow;
Servo servo_Shoulder;
Servo servo_Waist;
//for communication
char inDataBT[20]; // Allocate some space for the string
char inDataSerial[20]; // Allocate some space for the string
char inCharBT; // Where to store the character read
byte indexBT = 0; // Index into array; where to store the character
char inCharSerial; // Where to store the character read
byte indexSerial = 0; // Index into array; where to store the character
boolean cleanupBT;
boolean cleanupSerial;

NeoSWSerial BTSerial(RxD, TxD);

void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

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
  Serial.println( "wxx# servo waist x degree");
  Serial.println( "sxx# servo shoulder x degree");
  Serial.println( "exx# servo elbow x degree");
  Serial.println( "gxx# gripper x degree");
  Serial.println( "-----------------------------" );
}

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400);  
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  cleanupBT = false;
  cleanupSerial = false;
  servo_Gripper.attach(servoPin_Gripper);
  servo_Elbow.attach(servoPin_Elbow);
  servo_Shoulder.attach(servoPin_Shoulder);
  servo_Waist.attach(servoPin_Waist);
  Serial.println("after init");
  printMenu();
}

void makeCleanupBT() {
   for (indexBT = 0; indexBT < 20; indexBT++) {
      inDataBT[indexBT] = '\0';
   }
   inCharBT = '0';
   indexBT = 0;
   cleanupBT = false;
}

void makeCleanupSerial() {
   for (indexSerial = 0; indexSerial < 20; indexSerial++) {
      inDataSerial[indexSerial] = '\0';
   }
   inCharSerial = '0';
   indexSerial = 0;
   cleanupSerial = false;
}

void cleanup(bool isBT) {
  if (isBT) {
    makeCleanupBT();
  } else {
    makeCleanupSerial();
  }
}

bool makeMove(char *inData,int index, bool isBT) {
  if (inData[0] == 'w') {
     //remove w from command
     for (int i = 0 ; i < strlen(inData); i++) {
       inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
        cleanup(isBT);
        return false;
     }
     Serial.print("move servo waist ");
     Serial.print(atoi(inData));
     Serial.println("degree");
     servo_Waist.write(atoi(inData));
     cleanup(isBT);
     return true;
  } else if (inData[0] == 's') {
     //remove s from command
     for (int i = 0 ; i < strlen(inData); i++) {
        inData[i]=inData[i+1];
     }
     if (!isValidNumber(inData, index - 2)) {
        cleanup(isBT);
        return false;
     }
     Serial.print("move servo shoulder ");
     Serial.print(atoi(inData));
     Serial.println("degree");
     servo_Shoulder.write(180-atoi(inData));
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
     Serial.print("move servo elbow ");
     Serial.print(atoi(inData));
     Serial.println("degree");
     servo_Elbow.write(atoi(inData));
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
   
   while(BTSerial.available() > 0) // Don't read unless there you know there is data
   {
     if(indexBT < 19) // One less than the size of the array
     {
        inCharBT = BTSerial.read(); // Read a character
        if (inCharBT == '\r' || inCharBT == '\n' || inCharBT == '\0' || inCharBT < 35 || inCharBT > 122) {
          continue;
        }
        //commands start with a letter capital or small
        if (indexBT == 0 && !((inCharBT > 64 && inCharBT < 91) || (inCharBT > 96 && inCharBT < 123))) {
          continue;
        }    
        inDataBT[indexBT++] = inCharBT; // Store it
        inDataBT[indexBT] = '\0'; // Null terminate the string
        if (inCharBT == '#') {
          break;
        }
     } else {
        cleanup(true);
     }
   }
   if (inDataSerial[indexSerial-1] == '#') {
      if (indexSerial > 0) {
         inDataSerial[indexSerial-1] = '\0';
      }
      if (!makeMove(inDataSerial,indexSerial, false)) {        
         printMenu();
      }
   } else if (inDataBT[indexBT-1] == '#') {
      if (indexBT > 0) {
         inDataBT[indexBT-1] = '\0';
      }
      if (!makeMove(inDataBT, indexBT, true)) {
         printMenu();
      }
   }
}

void loop() {
   while( !Serial.available() && !BTSerial.available())
        ; // LOOP...
   readData();
}
