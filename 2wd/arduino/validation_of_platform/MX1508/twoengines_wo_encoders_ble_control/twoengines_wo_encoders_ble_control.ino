/*
Two engines without encoders controlled by BlueTooth using MX1508
Copyright 2020 Gabriel Dimitriu

This file is part of Robotics

It has also for testing purpose ISR with encoders on the two engines.
This version will be used to see if all connections are ok, encoders
ble etc. After everithing is ok the data will be put into 
twoengiens_ble_calibrations variables.

Robotics is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Robotics is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Robotics; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#define USE_PINCHANGEINTERRUPT
#include <NeoSWSerial.h>
#ifdef USE_PINCHANGEINTERRUPT
#include <PinChangeInt.h>
#else
#include <EnableInterrupt.h>
#endif
#define RxD 2
#define TxD 3
#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 9
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11
#define LEFT_ENCODER 4
#define RIGHT_ENCODER 12
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
volatile long left_encoder_count = 0;
volatile long right_encoder_count = 0;

#define SERIAL_DEBUG_MODE true
#define BLE_DEBUG_MODE true

NeoSWSerial BTSerial(RxD, TxD);
/*
 * isr for encoder pins
 */
void isrLeftEncoder() {
  left_encoder_count++;
}

void isrRightEncoder() {
  right_encoder_count++;
}

void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

void printMenuOnSerial() {
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "Calibration of engines with encoder using driver MX1508");
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "h# print this menu");
  Serial.println( "s# Stop" );
  Serial.println( "p# print encoder counting");
  Serial.println( "c# reset counters");
  Serial.println( "fxxx# full forward with xxx power util stop command");
  Serial.println( "bxx# move backward with xxx power until stop command");
  Serial.println( "lxxx# rotate left with xxx power until stop command");
  Serial.println( "xxxr# rotate right with xxx power until stop command");
  Serial.println( "-----------------------------" );
}

void printMenuOnBLE() {
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "Calibration of engines with encoder using driver MX1508\n");
  BTSerial.println( "-----------------------------------------------------\n" );
  BTSerial.println( "MENU:\n" );
  BTSerial.println( "h# print this menu\n");
  BTSerial.println( "s# Stop\n" );
  BTSerial.println( "p# print encoder counting\n");
  BTSerial.println( "c# reset counters\n");
  BTSerial.println( "fxxx# full forward with xxx power util stop command\n");
  BTSerial.println( "bxxx# move backward with xxx power until stop command\n");
  BTSerial.println( "lxxx# rotate left with xxx power until stop command\n");
  BTSerial.println( "rxxx# rotate right with xxx power until stop command\n");
  BTSerial.println( "-----------------------------\n" );
}

void setup() {

#ifdef SERIAL_DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");
#endif
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);
#ifdef USE_PINCHANGEINTERRUPT
  attachPinChangeInterrupt(LEFT_ENCODER, isrLeftEncoder, RISING);
  attachPinChangeInterrupt(RIGHT_ENCODER, isrRightEncoder, RISING);
  attachPinChangeInterrupt(RxD, neoSSerial1ISR, CHANGE);
#else
  enableInterrupt(LEFT_ENCODER, isrLeftEncoder, RISING);
  enableInterrupt(RIGHT_ENCODER, isrRightEncoder, RISING);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
#endif
  speedLeftEngine = 0;
  speedRightEngine = 0;
  cleanupBT = false;
  go(0,0);
#ifdef SERIAL_DEBUG_MODE
  printMenuOnSerial();
#endif
  printMenuOnBLE();
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
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(speedLeft); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(0); Serial.print(" , "); Serial.println(-speedLeft);
#endif    
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(speedRight); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
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
#ifdef BLE_DEBUG_MODE
    BTSerial.println("stop");
#endif
    go(0,0);
  } else if (strcmp(inData,"h") == 0) {
#ifdef SERIAL_DEBUG_MODE
    printMenuOnSerial();
#endif
    printMenuOnBLE();
  } else if (strcmp(inData,"c") == 0) {
#ifdef SERIAL_DEBUG_MODE
    Serial.println("Clear encoders, actual values are :");
    Serial.print("left encoder="); Serial.println(left_encoder_count);
    Serial.print("right encoder="); Serial.println(right_encoder_count);
#endif
    BTSerial.println("Clear encoders, actual values are :");
    BTSerial.print("left encoder="); BTSerial.println(left_encoder_count);
    BTSerial.print("right encoder="); BTSerial.println(right_encoder_count);
  } else if (strcmp(inData,"p") == 0) {
#ifdef SERIAL_DEBUG_MODE        
    Serial.print("left encoder="); Serial.println(left_encoder_count);
    Serial.print("right encoder="); Serial.println(right_encoder_count);
#endif
    BTSerial.print("left encoder="); BTSerial.println(left_encoder_count);
    BTSerial.print("right encoder="); BTSerial.println(right_encoder_count);
    left_encoder_count = 0;
    right_encoder_count = 0;
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
              inData[index++] = inChar; // Store it
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
