/*
  4 whell autonomous droid using bluetooth.
  This has also encoders and distance check and moving.
  Copyright 2020 Gabriel Dimitriu

  This file is part of Droids project

  Droid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  Droid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with Droids; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <VL53L0X.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
#include <Arduino.h>

#define RxD 2
#define TxD 3

#define BLE_DEBUG_MODE true
#define BLE_DEBUG_SETTINGS_MODE true

#define LEFT_FRONT_ENCODER_PIN 4
#define RIGHT_FRONT_ENCODER_PIN 7
#define LEFT_BACK_ENCODER_PIN 8
#define RIGHT_BACK_ENCODER_PIN 9

//pins on pca8695
#define LEFT_FRONT_MOTOR_PIN1 2
#define LEFT_FRONT_MOTOR_PIN2 3
#define RIGHT_FRONT_MOTOR_PIN1 0
#define RIGHT_FRONT_MOTOR_PIN2 1

#define LEFT_BACK_MOTOR_PIN1 13
#define LEFT_BACK_MOTOR_PIN2 12
#define RIGHT_BACK_MOTOR_PIN1 15
#define RIGHT_BACK_MOTOR_PIN2 14

#define FRONT_SERVO_PIN 11
#define SERVO_LEFT_45 410
#define SERVO_RIGHT_45 200
#define SERVO_CENTER 310

float PPI_front_left = 20/(2*PI*3.5f);
float PPI_front_right = 20/(2*PI*3.5f);
float PPI_back_left = 20/(2*PI*3.5f);
float PPI_back_right = 20/(2*PI*3.5f);

int countRotate90Left = 1;
int countRotate90Right = 1;
int countRotate1Inner = 1;
int countRotate1Outer = 1;

//variables for encoders
volatile long left_front_encoder_count = 0;
volatile long right_front_encoder_count = 0;
volatile long left_back_encoder_count = 0;
volatile long right_back_encoder_count = 0;

int btCommand;

int maxPower = 4000;
int slowPower = 1000;

// for input data from ble
boolean cleanupBT;
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);
VL53L0X frontSensor;
#define SAFE_DISTANCE 50 //minimum distance in mm
#define SLOW_DISTANCE 300 // the maximum distance when we go with slow power

NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}
/*
 * isr for encoder pins
 */
void isrLeftFrontEncoder() {
  left_front_encoder_count++;
}

void isrRightFrontEncoder() {
  right_front_encoder_count++;
}
void isrLeftBackEncoder() {
  left_back_encoder_count++;
}

void isrRightBackEncoder() {
  right_back_encoder_count++;
}

void moveBacwardLinear(float distance) {
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  int power = -maxPower;
  if (distance < 0) {
    distance = -distance;
    if (distance < 30) {
      power = power/2;
      if (power < -slowPower) {
        power = -slowPower;
      }
    }
    go(power, power);
    
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2 ){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2 ) {
        currentRightFrontPosition = right_front_encoder_count/PPI_front_right;
      } else {
        stopRightEngines();
        stopRight=true;
      }
    }
  }
  breakAllEngines();
  delay(100);
  go(0,0);
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftFrontPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightFrontPosition);
#endif  
}

void moveForwardLinear(float distance) {
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  int power = maxPower;
  boolean isGoingSlower = false;
  pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
  if (distance > 0) {
    if ( frontSensor.readRangeContinuousMillimeters() < SLOW_DISTANCE || distance < 30) {
      power = power/2;
      if (power > slowPower) {
        power = slowPower;
      }
      isGoingSlower = true;      
    }
    go(power, power);
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    uint16_t obstacle = frontSensor.readRangeContinuousMillimeters();
    if ( !isGoingSlower  && obstacle < SLOW_DISTANCE) {
      power = power/2;
      if (power > slowPower) {
        power = slowPower;
      }      
      go(power,power);
      isGoingSlower = true;
    }
    if (obstacle <= SAFE_DISTANCE) {
      breakAllEngines();
      delay(100);
      stopLeft=true;
      stopRight=true;
    }
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2 ){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    } else {
      stopLeftEngines();
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2) {
        currentRightFrontPosition = right_front_encoder_count/PPI_front_right;
      } else {
        stopRightEngines();
        stopRight=true;
      }
    } else {
      stopRightEngines();
    }
  }
  breakAllEngines();
  delay(100);
  go(0,0);
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftFrontPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightFrontPosition);
#endif  
}

void rotateDegree(long nr) {
  boolean stopLeftFront = false;
  boolean stopRightFront = false;
  boolean stopLeftBack = false;
  boolean stopRightBack = false;
  if (nr < 0) {
    go(-maxPower,maxPower);
    nr = -nr;
    while(!stopLeftFront || !stopLeftBack || !stopRightFront || !stopRightBack ) {
      if (!stopLeftFront && left_front_encoder_count >=  countRotate1Inner*nr) {
        stopLeftFront = true;
        breakLeftFrontEngine();
      }
      if (!stopRightFront && right_front_encoder_count >= countRotate1Outer*nr) {
        stopRightFront = true;
        breakRightFrontEngine();
      }
      if (!stopLeftBack && left_back_encoder_count >=  countRotate1Inner*nr) {
        stopLeftBack = true;
        breakLeftBackEngine();
      }
      if (!stopRightBack && right_back_encoder_count >= countRotate1Outer*nr) {
        stopRightBack = true;
        breakRightBackEngine();
      }
    }
  } else if (nr > 0) {
    go(maxPower,-maxPower);
    while(!stopLeftFront || !stopLeftBack || !stopRightFront || !stopRightBack ) {
      if (!stopLeftFront && left_front_encoder_count >= countRotate1Outer*nr) {
        stopLeftFront = true;
        breakLeftFrontEngine();
      }
      if (!stopRightFront && right_front_encoder_count >= countRotate1Inner*nr) {
        stopRightFront = true;
        breakRightFrontEngine();
      }
      if (!stopLeftBack && left_back_encoder_count >= countRotate1Outer*nr) {
        stopLeftBack = true;
        breakLeftBackEngine();
      }
      if (!stopRightBack && right_back_encoder_count >= countRotate1Inner*nr) {
        stopRightBack = true;
        breakRightBackEngine();
      }
    }
  } else {
    return;
  }
  breakAllEngines();
  delay(100);  
  go(0,0);
}

void breakLeftFrontEngine() {
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 4095);
}

void breakLeftBackEngine() {
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 4095);
}

void stopLeftEngines() {
  breakLeftFrontEngine();
  breakLeftBackEngine();
}

void breakRightFrontEngine() {
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 4095);
}

void breakRightBackEngine() {
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 4095);
}
void stopRightEngines() {
  breakRightFrontEngine();
  breakRightBackEngine();
}
void breakAllEngines() {
  stopLeftEngines();
  stopRightEngines();
}
/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
#ifdef BLE_DEBUG_MODE
    BTSerial.print("leftPower=");BTSerial.print(speedLeft);
    BTSerial.print(" rightPower=");BTSerial.println(speedRight);
#endif
  if (speedLeft == 0 && speedRight == 0 ) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
    return;
  }
  if (speedLeft > 0) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
  } 
  else {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, -speedLeft);    
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, -speedLeft);    
  }
 
  if (speedRight > 0) {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
  }else {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);    
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, -speedRight);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, -speedRight);
  }
}
static const char *pMenu = "MENU:";
static const char *pStop_Menu = "s# Stop";
static const char *pForwardFullMenu = "f# forward util stop";
static const char *pBackFullMenu = "b# backward until stop";
static const char *pForwardMenu = "fxx# move forward xx cm";
static const char *pBackMenu = "bxx# bacward xx cm";
static const char *pLeftMenu = "l# left until stop";
static const char *pRightMenu = "r# right until stop";
static const char *pRotateMenu = "rxx# rotate - for left + for right";
static const char *pPrintClearEncoders = "c# print and clear encoders";
static const char *pChangePower = "vxx# change maxPower";
void printMenu() {
    BTSerial.println( pMenu);
    BTSerial.println( pStop_Menu);
    BTSerial.println( pForwardFullMenu);
    BTSerial.println( pForwardMenu);
    BTSerial.println( pBackFullMenu);
    BTSerial.println( pBackMenu);
    BTSerial.println( pLeftMenu);
    BTSerial.println( pRotateMenu);
    BTSerial.println( pRightMenu);
    BTSerial.println( pPrintClearEncoders);
    BTSerial.println( pChangePower);
}

void setup()
{
    //init the PWM driver
    Wire.begin();
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);    
    //enable BLE
    BTSerial.begin(38400);
    enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
    isValidInput = false;
    //enable encoders ISR
    pinMode(LEFT_FRONT_ENCODER_PIN, INPUT_PULLUP);
    pinMode(RIGHT_FRONT_ENCODER_PIN, INPUT_PULLUP);
    pinMode(LEFT_BACK_ENCODER_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BACK_ENCODER_PIN, INPUT_PULLUP);
    enableInterrupt(LEFT_FRONT_ENCODER_PIN, isrLeftFrontEncoder, RISING);
    enableInterrupt(RIGHT_FRONT_ENCODER_PIN, isrRightFrontEncoder, RISING);
    enableInterrupt(LEFT_BACK_ENCODER_PIN, isrLeftBackEncoder, RISING);
    enableInterrupt(RIGHT_BACK_ENCODER_PIN, isrRightBackEncoder, RISING);
    printMenu();
    cleanupBT = false;
    maxPower=4000;
    frontSensor.init();
    frontSensor.startContinuous();
    //put the front servo to center
    pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
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

boolean makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
  BTSerial.flush();
}

void resetCounters() {
  left_front_encoder_count = 0;
  right_front_encoder_count = 0;
  left_back_encoder_count = 0;
  right_back_encoder_count = 0;
}

void printFrom() {
   BTSerial.print(" from ");
}
boolean makeMove() {
     if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strlen(inData) == 1) {
      if (inData[0] == 'c') {
        BTSerial.print("Left Front=");BTSerial.print(left_front_encoder_count);
        BTSerial.print("Left Back=");BTSerial.print(left_back_encoder_count);
        BTSerial.print("Right Front=");BTSerial.print(right_front_encoder_count);
        BTSerial.print("Right Back=");BTSerial.println(right_back_encoder_count);
        resetCounters();
      } else if (inData[0] == 'f') {
#ifdef BLE_DEBUG_MODE
        BTSerial.println("forward");
#endif
        go(maxPower, maxPower);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 's') {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("stop");
#endif
        breakAllEngines();
        delay(10);
        go(0, 0);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0]== 'b') {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("backward");
#endif
        go(-maxPower, -maxPower);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'l' ) {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("rotate left");
#endif
        go(-maxPower,maxPower);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'r') {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("rotate right");
#endif
        go(maxPower,-maxPower);
        makeCleanup();
        isValidInput = true;
     }
    } else if (strlen(inData) > 1) {
        if (inData[0] == 'f') {
        //remove f from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving forward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif
        resetCounters();    
        moveForwardLinear(atof(inData));
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Rotate ");
        BTSerial.print(atol(inData));
        BTSerial.println(" degree");
#endif
        resetCounters();
        rotateDegree(atol(inData));
        makeCleanup();        
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving backward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif
        resetCounters();
        moveBacwardLinear(-atof(inData));
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'v') {
        //remove v from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
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
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(maxPower);
#endif
        maxPower = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else {
        makeCleanup();
        printMenu();
        isValidInput = false;
      }
    } else {
      makeCleanup();
      printMenu();
      isValidInput = false;
    }
    makeCleanup();
    return true;
}

void loop()
{

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
    delay(5);
}
