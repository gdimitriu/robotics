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
#include "StringList.h"

#define RxD 2
#define TxD 3
#define servoPin_Gripper 4
#define servoPin_Elbow 5
#define servoPin_Shoulder 6
#define servoPin_Waist 7
#define SERIAL_DEBUG 1

Servo servo_Gripper;
Servo servo_Elbow;
Servo servo_Shoulder;
Servo servo_Waist;
//for communication
char inDataBT[20]; // Allocate some space for the string
char inCharBT; // Where to store the character read
byte indexBT = 0; // Index into array; where to store the character
boolean cleanupBT;
StringList commands;

NeoSWSerial BTSerial(RxD, TxD);
const int prefixCommandNr = 5;
const char prefixCommands[prefixCommandNr] = {'w','s','e','g','d'};


void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

boolean isValidNumber(char *data) {
  if (strlen(data) == 0 ) return false;
#ifdef SERIAL_DEBUG
  Serial.println(data);
#endif  
  if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

  for(byte i=0;i<strlen(data);i++) {
    if(!(isDigit(data[i]) || data[i] == '.')) return false;
  }
  return true;
}

#ifdef SERIAL_DEBUG
void printMenu() {
  if (!Serial)
    return;
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "Mini Robot arm");
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "wxx# servo waist xx degree");
  Serial.println( "sxx# servo shoulder xx degree");
  Serial.println( "exx# servo elbow xx degree");
  Serial.println( "gxx# gripper xx degree");
  Serial.println( "dxx# delay of xx miliseconds");
  Serial.println( "S[w/s/e/g/d]xx# save the command");
  Serial.println( "Rf# run in forward order");
  Serial.println( "Rr# run in reverse order");
  Serial.println( "C# clear the saved commands");
  Serial.println( "-----------------------------" );
}
#endif

void setup() {
#ifdef SERIAL_DEBUG  
  Serial.begin(9600);
#endif  
  BTSerial.begin(38400);  
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  cleanupBT = false;
  servo_Gripper.attach(servoPin_Gripper);
  servo_Elbow.attach(servoPin_Elbow);
  servo_Shoulder.attach(servoPin_Shoulder);
  servo_Waist.attach(servoPin_Waist);
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.println("after init");
    printMenu();
  }
#endif
}

void makeCleanupBT() {
  BTSerial.println("OK");
  BTSerial.flush();
   for (indexBT = 0; indexBT < 20; indexBT++) {
      inDataBT[indexBT] = '\0';
   }
   inCharBT = '0';
   indexBT = 0;
   cleanupBT = false;
}

bool isPrefixCommand(char prefix) {
  for(int i = 0 ;i < prefixCommandNr; i++) {
    if (prefix == prefixCommands[i])
      return true;
  }
  return false;
}

bool moveWaist(char *inData) {
  //remove w from command
  for (int i = 0 ; i < strlen(inData); i++) {
     inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData)) {
     return false;
  }
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.print("move servo waist ");
    Serial.print(atoi(inData));
    Serial.println("degree");
  }
#endif     
  servo_Waist.write(atoi(inData));
  return true;  
}

bool moveShoulder(char *inData) {
  //remove s from command
  for (int i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData)) {
    return false;
  }
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.print("move servo shoulder ");
    Serial.print(atoi(inData));
    Serial.println("degree");
  }
#endif     
  servo_Shoulder.write(180-atoi(inData));
  return true;
}

bool moveElbow(char *inData) {
  //remove e from command
  for (int i = 0 ; i < strlen(inData); i++) {
     inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData)) {
     return false;
  }
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.print("move servo elbow ");
    Serial.print(atoi(inData));
    Serial.println("degree");
  }
#endif     
  servo_Elbow.write(atoi(inData));
  return true;
}

bool moveGripper(char *inData) {
  //remove g from command
  for (int i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData)) {
    return false;
  }
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.print("move servo grabber ");
    Serial.print(atoi(inData));
    Serial.println("degree");
  }
#endif     
  servo_Gripper.write(atoi(inData));
  return true;
}

bool applyDelay(char *inData) {
  //remove d from command
  for (int i = 0 ; i < strlen(inData); i++) {
    inData[i]=inData[i+1];
  }
  if (!isValidNumber(inData)) {
    return false;
  }
#ifdef SERIAL_DEBUG
  if (Serial) {
    Serial.print("sleep ");
    Serial.print(atol(inData));
    Serial.println("miliseconds");
  }
#endif     
  delay(atol(inData));
  return true;
}

bool makeMove(char *inData, bool isBT) {
  bool retValue = false;
  if (inData[0] == 'w') {
    retValue = moveWaist(inData);
  } else if (inData[0] == 's') {
    retValue = moveShoulder(inData);
  } else if (inData[0] == 'e') {
    retValue = moveElbow(inData);
  } else if (inData[0] == 'g') {
    retValue = moveGripper(inData);
  } else if (inData[0] == 'd') {
    retValue = applyDelay(inData);
  } else if (inData[0] == 'S') {
    //remove S from command
    for (int i = 0 ; i < strlen(inData); i++) {
       inData[i]=inData[i+1];
    }
    if (isPrefixCommand(inData[0])) {
      char *temp = inData[1];
      if (!isValidNumber(temp)) {
        retValue = false;
      } else {
        commands.addTail(inData);
        retValue = true;
      }
    }
  } else if (inData[0] == 'C') {
    commands.clear();
    retValue = true;
  } else if (inData[0] == 'R') {
    //remove R from command
    for (int i = 0 ; i < strlen(inData); i++) {
       inData[i]=inData[i+1];
    }
    if (inData[0] == 'f') {
#ifdef SERIAL_DEBUG
      if (Serial) {
        Serial.println("Move commands in forward order");
      }
#endif      
      commands.reset();
      char *temp = commands.getForwardValue();
      while (temp != NULL) {
        makeMove(temp,false);
        temp = commands.getForwardValue();
      }
    } else if (inData[0] == 'r') {
#ifdef SERIAL_DEBUG
      if (Serial) {
        Serial.println("Move commands in reverse order");
      }
#endif
      commands.reset();
      char *temp = commands.getReverseValue();
      while (temp != NULL) {
        makeMove(temp,false);
        temp = commands.getReverseValue();
      }
    }
  }
  if (isBT) {
    makeCleanupBT();
  }
  return retValue;
}

void readData() {   
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
        makeCleanupBT();
     }
   }
   if (indexBT > 0 && inDataBT[indexBT-1] == '#') {
     inDataBT[indexBT-1] = '\0';
     if (!makeMove(inDataBT, true)) {
#ifdef SERIAL_DEBUG        
       printMenu();
#endif         
     }
   }
}

void loop() {
   while(!BTSerial.available())
        ; // LOOP...
   readData();
}
