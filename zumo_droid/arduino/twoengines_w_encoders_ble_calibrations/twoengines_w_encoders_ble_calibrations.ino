/*
 * Calibration of engines using encoder phase A and MX1508 driver with ble.
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

#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 5
#define LEFT_MOTOR_PIN2 6
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11
#define LEFT_ENCODER 4
#define RIGHT_ENCODER 12

float whellRadius = 2;
long resolutionCodor_left = 1452;
long resolutionCodor_right = 1468;
const float pi = 3.14f;
float PPI_left = resolutionCodor_left/(2*pi*whellRadius);
float PPI_right = resolutionCodor_right/(2*pi*whellRadius);
long countRotate90Left= 841;
long countRotate90Right= 830;
long countRotate1Inner = 9;
long countRotate1Outer = 10;

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
volatile long left_encoder_count = 0;
volatile long right_encoder_count = 0;
float currentLeftPosition = 0.0f;
float currentRightPosition = 0.0f;
// for input data from ble
boolean cleanupBT;

long speedValue = 255;

NeoSWSerial BTSerial(RxD, TxD);

//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

#define SERIAL_DEBUG_MODE true

/*
 * isr for encoder pins
 */
void isrLeftEncoder() {
  left_encoder_count++;
}

void isrRightEncoder() {
  right_encoder_count++;
}
void printMenu() {
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "Calibration of engines with encoder using driver MX1508");
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "MENU:" );
  BTSerial.println( "s# Stop" );
  BTSerial.println( "p# print encoder counting");
  BTSerial.println( "c# reset counters");
  BTSerial.println( "f# forward util stop command");
  BTSerial.println( "b# backward until stop command");
  BTSerial.println( "h# print help");
  BTSerial.println( "fxx# move forward xx cm");
  BTSerial.println( "bxx# move backward xx cm");
  BTSerial.println( "l# rotate 90 left");
  BTSerial.println( "r# rotate 90 right");
  BTSerial.println( "lxx# rotate xx degree left");
  BTSerial.println( "rxx# rotate xx degree right");
  BTSerial.println( "vxx# change speed value");
  BTSerial.println( "-----------------------------" );
}

void setup() {
#ifdef SERIAL_DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");
#endif
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  isValidInput = false;
  enableInterrupt(LEFT_ENCODER, isrLeftEncoder, RISING);
  enableInterrupt(RIGHT_ENCODER, isrRightEncoder, RISING);
  cleanupBT = false;
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
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

void resetCounters() {
  left_encoder_count = 0;
  right_encoder_count = 0;
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

boolean makeMove() {
    if (strcmp(inData,"f") == 0) {
#ifdef BLE_DEBUG_MODE        
      BTSerial.println("forward");
#endif      
      go(speedValue, speedValue);
      delay(1000);
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif      
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"b") == 0) {
#ifdef BLE_DEBUG_MODE        
      BTSerial.println("backward");
#endif      
      go(-speedValue, -speedValue);
      delay(1000);
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif      
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"c") == 0) {
      BTSerial.println("reset counters");
      resetCounters();
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0 ) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate 90 degree left");
#endif      
      rotate90Left();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate 90 degree right");
#endif      
      rotate90Right();
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0 ) {
        BTSerial.print("EncoderValues left="); BTSerial.print(left_encoder_count);
        BTSerial.print(" right=");BTSerial.println(right_encoder_count);        
        isValidInput = true;
    } else if (strcmp(inData,"h") == 0) {
      printMenu();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Moving forward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif        
        moveForward(atof(inData));
        isValidInput = true;
      } else if (inData[0] == 'l') {
          //remove l from command
          for (int i = 0 ; i < strlen(inData); i++) {
            inData[i]=inData[i+1];
          }
          if (!isValidNumber(inData, index - 2)) {
            isValidInput = false;
            makeCleanup();
            return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Rotate left  ");
        BTSerial.print(atol(inData));
        BTSerial.println(" degree");
#endif        
        rotateLeftDegree(atol(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Rotate right  ");
        BTSerial.print(atol(inData));
        BTSerial.println(" degree");
#endif        
        rotateRightDegree(atol(inData));
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Moving backward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif        
        moveBackward(atof(inData));
        isValidInput = true;
      } else if (inData[0] == 'v') {
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

void rotate90Left() {
  go(-speedValue,speedValue);
  while(left_encoder_count < countRotate90Left && right_encoder_count < countRotate90Right);
  go(0,0);
}

void rotate90Right() {
  go(speedValue,-speedValue);
  while(left_encoder_count < countRotate90Left && right_encoder_count < countRotate90Right);
  go(0,0);
}

void rotateLeftDegree(long nr) {
  go(-speedValue,speedValue);
  while(left_encoder_count < countRotate1Inner*nr && right_encoder_count < countRotate1Outer*nr);
  go(0,0);
}

void rotateRightDegree(long nr) {
  go(speedValue,-speedValue);
  while(left_encoder_count < countRotate1Outer*nr && right_encoder_count < countRotate1Inner*nr);
  go(0,0);
}

void moveForward(float distance) {
  go(speedValue,speedValue);
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (!stopLeft) {
      if((distance - currentLeftPosition) > 0.2){
        currentLeftPosition = left_encoder_count/PPI_left;
      } else {
        stopLeftEngine();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightPosition) > 0.2) {
        currentRightPosition = right_encoder_count/PPI_right;
      } else {
        stopRightEngine();
        stopRight=true;
      }
    }
  }
  go(0,0);
#ifdef SERIAL_DEBUG_MODE
  Serial.print("Current position="); Serial.print(distance);
  Serial.print(" leftPosition="); Serial.print(currentLeftPosition);
  Serial.print(" rightPosition="); Serial.println(currentRightPosition);
#endif
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightPosition);
#endif
  currentLeftPosition = 0;
  currentRightPosition = 0;
}

void moveBackward(float distance) {
  go(-speedValue,-speedValue);
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (!stopLeft) {
      if((distance - currentLeftPosition) > 0.2){
        currentLeftPosition = left_encoder_count/PPI_left;
      } else {
        stopLeftEngine();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightPosition) > 0.2) {
        currentRightPosition = right_encoder_count/PPI_right;
      } else {
        stopRightEngine();
        stopRight=true;
      }
    }
  }
  go(0,0);
#ifdef SERIAL_DEBUG_MODE
  Serial.print("Current position="); Serial.print(distance);
  Serial.print(" leftPosition="); Serial.print(currentLeftPosition);
  Serial.print(" rightPosition="); Serial.println(currentRightPosition);
#endif
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightPosition);
#endif
  currentLeftPosition = 0;
  currentRightPosition = 0;
}

void stopLeftEngine() {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
}

void stopRightEngine() {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
}
