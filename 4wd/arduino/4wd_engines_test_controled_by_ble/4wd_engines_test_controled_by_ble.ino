/*

Calibration of engines of 4WD chasy using BLE

This used to see if your engines are working as expected and wire are
done correctly.
Thi also test if you BLE connection is working as expected.

Copyright 2020 Gabriel Dimitriu

This file is part of Droids

Droids is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Droids is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Droids; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define RxD 2
#define TxD 3
int LEFT_FRONT_MOTOR_PIN1 = 2;
int LEFT_FRONT_MOTOR_PIN2 = 3;
int RIGHT_FRONT_MOTOR_PIN1 = 0;
int RIGHT_FRONT_MOTOR_PIN2 = 1;

int LEFT_BACK_MOTOR_PIN1 = 13;
int LEFT_BACK_MOTOR_PIN2 = 12;
int RIGHT_BACK_MOTOR_PIN1 = 15;
int RIGHT_BACK_MOTOR_PIN2 = 14;

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

long maxPower = 4000;
boolean cleanupBT;

NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}
Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

void printMenu()
{
  BTSerial.println( "-----------------------------------------------------------------------" );
  BTSerial.println( "Calibration of 4wd engines using driver L298N micro and extender PCA9685");
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "MENU:" );
  BTSerial.println( "s# Stop" );
  BTSerial.println( "fx# full forward util stop command where x is engine number");
  BTSerial.println( "bx# full backward until stop command whre x is engine number");
  BTSerial.println( "vxx# change speed value");
  BTSerial.println( "-----------------------------------------------------------------------" );
}

void setup() {
  isValidInput = false;
  BTSerial.begin(38400);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  BTSerial.println("Starting...");
  pwmDriver.begin();
  pwmDriver.setOscillatorFrequency(27000000);
  pwmDriver.setPWMFreq(50.0);
  Wire.setClock(400000);
  cleanupBT = false;
  printMenu();
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

void stopAllEngines(){
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);    
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
  BTSerial.println("Stop all engines");
  return;
}

void moveLeftFrontEngine(int speedValue) {
  BTSerial.print("power left front engine=");
  BTSerial.println(speedValue);
  if (speedValue > 0) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, speedValue);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
  } else {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, -speedValue);
  }
}

void moveLeftBackEngine(int speedValue) {
  BTSerial.print("power left back engine=");
  BTSerial.println(speedValue);
  if (speedValue > 0) {
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, speedValue);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
  } else {
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, -speedValue);
  }
}

void moveRightFrontEngine(int speedValue) {
  BTSerial.print("power right front engine=");
  BTSerial.println(speedValue);
  if (speedValue > 0) {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, speedValue);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);
  } else {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, -speedValue);
  }
}

void moveRightBackEngine(int speedValue) {
  BTSerial.print("power right back engine=");
  BTSerial.println(speedValue);
  if (speedValue > 0) {
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, speedValue);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
  } else {
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, -speedValue);
  }
}

void moveEngine(int engineNr, int speedValue) {
  switch(engineNr) {
    case 1:
      moveLeftFrontEngine(speedValue);
      break;
    case 2:
      moveRightFrontEngine(speedValue);
      break;
    case 3:
      moveRightBackEngine(speedValue);
      break;
    case 4:
      moveLeftBackEngine(speedValue);
      break;
  }
}
boolean makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}
boolean makeMove() {
     if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData,"s") == 0) {
      BTSerial.println("stop");
      stopAllEngines();
      makeCleanup();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
    if (inData[0] == 'v') {
        //remove v from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(inData) > 4095 || atol(inData) < 0) {
          BTSerial.println("Speed value should be between 0 and 4095");
          isValidInput = false;
          makeCleanup();
          return false;
        }
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(maxPower);
        maxPower = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(inData) > 4 || atol(inData) < 0) {
          BTSerial.println("egine numbers 1 front left, 2 front right , 3 back right, 4 back left");
          isValidInput = false;
          makeCleanup();
          return false;
        }
        moveEngine(atoi(inData),maxPower);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(inData) > 4 || atol(inData) < 0) {
          BTSerial.println("egine numbers 1 front left, 2 front right , 3 back right, 4 back left");
          isValidInput = false;
          makeCleanup();
          return false;
        }
        moveEngine(atoi(inData),-maxPower);
        makeCleanup();
        isValidInput = true;
      } else {
        makeCleanup();
        isValidInput = false;
      }
    } else {
      makeCleanup();
      isValidInput = false;
    }
    makeCleanup();
    return true;
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
     printMenu();
  }
}
