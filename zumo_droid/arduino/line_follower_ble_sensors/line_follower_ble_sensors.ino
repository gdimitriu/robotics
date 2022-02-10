/*
 * Line follower and MX1508 driver with ble.
 *
 * Copyright 2021 Gabriel Dimitriu
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
#define BLE_DEBUG_MODE true

#include <EnableInterrupt.h>
#include <NeoSWSerial.h>
#include <QTRSensors.h>

#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 5
#define LEFT_MOTOR_PIN2 6
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11
#define LEFT_LINE_SENSOR A0
#define CENTER_LINE_SENSOR A1
#define RIGHT_LINE_SENSOR A2

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
// for input data from ble
boolean cleanupBT;
boolean followLine = false;

long speedValue = 255;
unsigned int lineSensors[3];

NeoSWSerial BTSerial(RxD, TxD);

QTRSensors sensors;;
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

void printMenu() {
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "Line follower using driver MX1508");
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "MENU:" );
  BTSerial.println( "s# Stop" );
  BTSerial.println( "C# calibrate sensors");
  BTSerial.println( "R# read sensors");
  BTSerial.println( "-----------------------------" );
}

void setup() {
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_LINE_SENSOR, INPUT_PULLUP);
  pinMode(CENTER_LINE_SENSOR, INPUT_PULLUP);
  pinMode(RIGHT_LINE_SENSOR, INPUT_PULLUP);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  sensors.setTypeAnalog();
  sensors.setSamplesPerSensor(20);
  sensors.setSensorPins((const uint8_t[]) {A0,A1,A2},3);
  isValidInput = false;
  cleanupBT = false;
  followLine = false;
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  printMenu();
//  int i;
//  for(i = 0; i < 250;i++) {
//    sensors.calibrate();
//    delay(20);
//  }
  lineSensors[0] = 0;
  lineSensors[1] = 0;
  lineSensors[2] = 0;
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

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void followTheLine() {
  
}

boolean makeMove() {
    if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif      
      go(0, 0);
      followLine = false;
      isValidInput = true;
    } else if (strcmp(inData,"h") == 0) {
      printMenu();
      isValidInput = true;
    } else if (strcmp(inData,"C") == 0) {
      BTSerial.println("Calibration of line sensors");
      sensors.resetCalibration();
      for(int i = 0; i < 250;i++) {
        sensors.calibrate();
        delay(20);
      }
      sensors.readCalibrated(lineSensors);
      BTSerial.print("sensor 0=");BTSerial.print(lineSensors[0]);
      BTSerial.print("sensor 1=");BTSerial.print(lineSensors[1]);
      BTSerial.print("sensor 2=");BTSerial.println(lineSensors[2]);
    } else if (strcmp(inData,"R") == 0) {
      BTSerial.println("Read of line sensors");
      sensors.readLineBlack(lineSensors);
      BTSerial.print("sensor 0=");BTSerial.print(lineSensors[0]);
      BTSerial.print("sensor 1=");BTSerial.print(lineSensors[1]);
      BTSerial.print("sensor 2=");BTSerial.println(lineSensors[2]);
    } else {
      makeCleanup(); 
      isValidInput = false;
      return false;
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
     inData[index-1] = '\0';
     makeMove();
     return;
  } else if (cleanupBT) {
     makeCleanup();
     cleanupBT = false;
     printMenu();
  }
  if (followLine)
    followTheLine();
}

void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.println("All on zero");
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedRight);
#endif
  }
}
