/*
 * Propellers controlled by BlueTooth using MOS-FET
 * Copyright 2020 Gabriel Dimitriu
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
#include <SoftwareSerial.h>
#define RxD 2
#define TxD 3
#define LEFT_MOTOR_PIN 5
#define RIGHT_MOTOR_PIN 6
//speed of engines
int speedLeftEngine;
int speedRightEngine;
//for communication
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inDataLeft[20];
char inDataRight[20];
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
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN, OUTPUT);
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
    digitalWrite(LEFT_MOTOR_PIN,LOW);
    digitalWrite(RIGHT_MOTOR_PIN,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.println("All on zero");
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN, speedLeft);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(speedLeft); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif    
  } else 
#ifdef SERIAL_DEBUG_MODE
    Serial.println("Left speed should be only positive");
#endif    
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Left speed should be only positive");
#endif
  }
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN, speedRight);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(speedRight); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
#ifdef SERIAL_DEBUG_MODE
    Serial.println("Right speed should be only positive");
#endif
#ifdef BLE_DEBUG_MODE
    Serial.println("Right speed should be only positive");
#endif
  }
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
    inDataLeft[index] = '\0';
    inDataRight[index] = '\0';
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
      } else if (inData[0] == 'd') {
        int indexLeft = 0;
        int indexRight = 0;
        boolean isRight = false;
        //remove d from command
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        for (unsigned int i = 0 ; i < strlen(inData); i++) {
          if (inData[i] == ',') {
            inDataLeft[indexLeft] = '\0';
            indexLeft++;
            isRight = true;
#ifdef BLE_DEBUG_MODE        
        BTSerial.println("Got ,");
#endif

          } else if (isRight) {
            inDataRight[indexRight] = inData[i];
            indexRight++;
          } else {
            inDataLeft[indexLeft] = inData[i];
            indexLeft++;
          }
        }
        inDataRight[indexRight] = '\0';
        indexRight++;
        if (!isValidNumber(inDataLeft, indexLeft-1) || !isValidNumber(inDataRight, indexRight-1)) {
          makeCleanup();
          return;
        }
        indexLeft = atoi(inDataLeft);
        indexRight = atoi(inDataRight);
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("front differential ");
        Serial.print(indexLeft); Serial.print(" : "); Serial.println(indexRight);        
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("front differential ");
        BTSerial.print(indexLeft); BTSerial.print(" : "); BTSerial.println(indexRight);
#endif
        go(indexLeft,indexRight);
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
