/*
 * Two engine controlled by L298N by BlueTooth
 * Copyright 2020 Gabriel Dimitriu
 *
 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
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
#include <SoftwareSerial.h>
#define RxD 2
#define TxD 3
int LEFT_MOTOR_PIN1 = 6;
int LEFT_MOTOR_PIN2 = 5;
int LEFT_MOTOR_SPEED = 4;
int RIGHT_MOTOR_PIN1 = 7;
int RIGHT_MOTOR_PIN2 = 8;
int RIGHT_MOTOR_SPEED = 9;
//speed of engines
int speedLeftEngine;
int speedRightEngine;
//for communication
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
boolean cleanupBT;
int tempIValue;

SoftwareSerial BTSerial(RxD, TxD);

#define SERIAL_DEBUG_MODE true
#define BLE_DEBUG_MODE true

void setup() {

#ifdef SERIAL_DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");
#endif
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  speedLeftEngine = 0;
  speedRightEngine = 0;
  cleanupBT = false;
  go(0,0);

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
/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.println("All on zero");
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,HIGH);
    analogWrite(LEFT_MOTOR_SPEED, speedLeft);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(speedLeft); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif   
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,HIGH);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(0); Serial.print(" , "); Serial.println(-speedLeft);
#endif    
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,HIGH);
    analogWrite(RIGHT_MOTOR_SPEED, speedRight);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(speedRight); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, -speedRight);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(0); Serial.print(" , "); Serial.println(-speedRight);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedRight);
#endif
  }
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void makeMove() {
  if (index > 0) {
     inData[index-1] = '\0';
  }
  if (strcmp(inData,"s") == 0) {
#ifdef SERIAL_DEBUG_MODE        
    Serial.println("stop");
#endif
    go(0,0);
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
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("front ");
        Serial.println(tempIValue);        
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("front ");
        BTSerial.println(tempIValue);        
#endif
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
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("backward ");
        Serial.println(tempIValue);        
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("backward ");
        BTSerial.println(tempIValue);        
#endif
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
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("rotate left with diferencial power  ");
        Serial.println(tempIValue);        
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("rotate left with diferencial power  ");
        BTSerial.println(tempIValue);        
#endif

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
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("rotate right with diferencial power ");
        Serial.println(tempIValue);        
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("rotate right with diferencial power ");
        BTSerial.println(tempIValue);        
#endif
        go(tempIValue,-tempIValue);
      }
    }
    makeCleanup();
}

void loop() {
      while(BTSerial.available() > 0) // Don't read unless there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = BTSerial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
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
}
