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
//#define BLE_MOVE_DEBUG
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

NeoSWSerial BTSerial(RxD, TxD);

QTRSensors sensors;
unsigned int lineSensors[3];

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
  BTSerial.println( "h# print help");
  BTSerial.println( "vxx# change speed value");
  BTSerial.println( "f# forward util stop command");
  BTSerial.println( "b# backward until stop command");
  BTSerial.println( "L# follow the line");
  BTSerial.println( "C# calibrate sensors");
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
  sensors.setSensorPins((const uint8_t[]) {A0,A1,A2},3);
  sensors.setSamplesPerSensor(16);
  isValidInput = false;
  cleanupBT = false;
  followLine = false;
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  printMenu();
  int i;
  for(i = 0; i < 250;i++) {
    sensors.calibrate();
    delay(20);
  }
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
  int position = sensors.readLineBlack(lineSensors);
  if (lineSensors[0] > 750 && lineSensors[1] > 750 && lineSensors[2] > 750) {
    go(0,0);
    followLine = false;
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Stopping");
#endif
    return;
  }
  int error = position - 1000;
#ifdef BLE_DEBUG_MODE
  BTSerial.println(error);  
#endif
  if (error < -500) // the line is on the left
    go(-200,200);
  else if (error > 500) // the line is on the right
    go(200,-200);
  else
    go(200,200);
}

boolean makeMove() {
    if (strcmp(inData,"f") == 0) {
#ifdef BLE_DEBUG_MODE        
      BTSerial.println("forward");
#endif      
      go(speedValue, speedValue);
      delay(1000);
      isValidInput = true;
    } else if (strcmp(inData,"b") == 0) {
#ifdef BLE_DEBUG_MODE        
      BTSerial.println("backward");
#endif      
      go(-speedValue, -speedValue);
      delay(1000);
      isValidInput = true;
    } else if (strcmp(inData,"L") == 0) {
#ifdef BLE_DEBUG_MODE        
      BTSerial.println("Line follower");
#endif
      followLine = true;
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
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
      for(int i = 0; i < 250;i++) {
        sensors.calibrate();
        delay(20);
      }
#ifdef BLE_DEBUG_MODE      
      sensors.readCalibrated(lineSensors);
      BTSerial.print("sensor 0=");BTSerial.print(lineSensors[0]);
      BTSerial.print("sensor 1=");BTSerial.print(lineSensors[1]);
      BTSerial.print("sensor 2=");BTSerial.print(lineSensors[2]);
#endif      
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
        if (atol(inData) > 255 || atol(inData) < 0) {
          BTSerial.println("Speed value should be between 0 and 255");
          isValidInput = false;
          makeCleanup(); 
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(speedValue);
#endif        
        speedValue = atol(inData);
        isValidInput = true;
      } else {
        isValidInput = false;
        makeCleanup(); 
        return false;
      }
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
#ifdef BLE_MOVE_DEBUG
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef BLE_MOVE_DEBUG
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef BLE_MOVE_DEBUG
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef BLE_MOVE_DEBUG
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef BLE_MOVE_DEBUG
    BTSerial.print("Right "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedRight);
#endif
  }
}
