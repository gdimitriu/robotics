/*
 * Engines with MOSFET type N 30N06L on quadcopter using ble.
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
/*
 * MOS-FET driver configurations
 * Gate to 10 k rezistor and to PWM pin
 * 10k rezistor to GND
 * drain to engine -
 * source to GND
 * engine + to VCC
 */
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

#define RxD 4
#define TxD 3

#define RIGHT_ENGINE_PIN 5
#define LEFT_ENGINE_PIN 6
#define FRONT_ENGINE_PIN 11
#define BACK_ENGINE_PIN 12

#define FRONT_ENGINE_IDX 0
#define RIGHT_ENGINE_IDX 1
#define BACK_ENGINE_IDX 2
#define LEFT_ENGINE_IDX 3

boolean cleanupBT;

int enginePower[4];

NeoSWSerial BTSerial(RxD, TxD);

void setup() {
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  pinMode(FRONT_ENGINE_PIN, OUTPUT);
  pinMode(RIGHT_ENGINE_PIN, OUTPUT);
  pinMode(BACK_ENGINE_PIN, OUTPUT);
  pinMode(LEFT_ENGINE_PIN, OUTPUT);  
  enginePower[FRONT_ENGINE_IDX]=0;
  enginePower[RIGHT_ENGINE_IDX]=0;
  enginePower[BACK_ENGINE_IDX]=0;
  enginePower[LEFT_ENGINE_IDX]=0;
  cleanupBT = false;
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
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

void powerToAllEngines(int value) {
  BTSerial.print(" Power all engines to value:");
  BTSerial.println(value);
  analogWrite(FRONT_ENGINE_PIN, value);
  analogWrite(RIGHT_ENGINE_PIN, value);
  analogWrite(BACK_ENGINE_PIN, value);
  analogWrite(LEFT_ENGINE_PIN, value);
  enginePower[FRONT_ENGINE_IDX]=value;
  enginePower[RIGHT_ENGINE_IDX]=value;
  enginePower[BACK_ENGINE_IDX]=value;
  enginePower[LEFT_ENGINE_IDX]=value;
}

void powerToFrontEngine(int value) {
  BTSerial.print("Power to front engine from : ");
  BTSerial.print(enginePower[FRONT_ENGINE_IDX]);
  BTSerial.print(" to : ");
  BTSerial.println(value);
  analogWrite(FRONT_ENGINE_PIN, value);
  enginePower[FRONT_ENGINE_IDX] = value;
}

void powerToRightEngine(int value) {
  BTSerial.print("Power to right engine from : ");
  BTSerial.print(enginePower[RIGHT_ENGINE_IDX]);
  BTSerial.print(" to : ");
  BTSerial.println(value);
  analogWrite(RIGHT_ENGINE_PIN, value);
  enginePower[RIGHT_ENGINE_IDX] = value;
}

void powerToBackEngine(int value) {
  BTSerial.print("Power to back engine from : ");
  BTSerial.print(enginePower[BACK_ENGINE_IDX]);
  BTSerial.print(" to : ");
  BTSerial.println(value);
  analogWrite(BACK_ENGINE_PIN, value);
  enginePower[BACK_ENGINE_IDX] = value;
}

void powerToLeftEngine(int value) {
  BTSerial.print("Power to left engine from : ");
  BTSerial.print(enginePower[LEFT_ENGINE_IDX]);
  BTSerial.print(" to : ");
  BTSerial.println(value);
  analogWrite(LEFT_ENGINE_PIN, value);
  enginePower[LEFT_ENGINE_IDX] = value;
}

void printAllEnginesLevel() {
  BTSerial.print("Front engine power level : "); BTSerial.println(enginePower[FRONT_ENGINE_IDX]);
  BTSerial.print("Right engine power level : "); BTSerial.println(enginePower[RIGHT_ENGINE_IDX]);
  BTSerial.print("Back engine power level : "); BTSerial.println(enginePower[BACK_ENGINE_IDX]);
  BTSerial.print("Left engine power level : "); BTSerial.println(enginePower[LEFT_ENGINE_IDX]);
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

void makeMove() {
   if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData,"s") == 0) {
      BTSerial.println("stop");
      powerToAllEngines(0);
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0) {
      printAllEnginesLevel();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'a') {
        //remove a from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return;
        }
        powerToAllEngines(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return;
        }
        powerToFrontEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return;
        }
        powerToRightEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return;
        }
        powerToBackEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'l') {
        //remove l from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return;
        }
        powerToLeftEngine(atoi(inData));
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
        isValidInput = false;
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
