/*
 * 4 whell autonomous droid using bluetooth.
 * This has also encoders and distance check and moving.
 * Copyright 2020 Gabriel Dimitriu
 *
 * This file is part of Robotics project

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
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 3

#define BLE_DEBUG_MODE true
#define BLE_DEBUG_AUTO_MODE true
#define BLE_DEBUG_SETTINGS_MODE true

#define trigPinR 5
#define echoPinR 6
#define trigPinL 10
#define echoPinL 11
#define LEFT_FRONT_ENCODER_PIN 4
#define RIGHT_FRONT_ENCODER_PIN 7
#define LEFT_BACK_ENCODER_PIN 8
#define RIGHT_BACK_ENCODER_PIN 9


#define LEFT_FRONT_MOTOR_PIN1 2
#define LEFT_FRONT_MOTOR_PIN2 3
#define RIGHT_FRONT_MOTOR_PIN1 0
#define RIGHT_FRONT_MOTOR_PIN2 1

#define LEFT_BACK_MOTOR_PIN1 13
#define LEFT_BACK_MOTOR_PIN2 12
#define RIGHT_BACK_MOTOR_PIN1 15
#define RIGHT_BACK_MOTOR_PIN2 14

#define setDistance 20
unsigned long autoTime;
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

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

//running time in auto mode
long dt;

int turatie;
bool sens;
int btCommand;
int mode = NULL;
bool lastDirection = true;

// variables used for PID algorithm
int maxPower;
int lastError = 0;
int error;
/*
 * those should be set using the Ziggler-Nichols method.
 */
float Kp, Ki, Kd;
int delayMove;

// for input data from ble
boolean cleanupBT;
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

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
long read(bool side)
{
    long durata, distanta;
    int trigPin, echoPin;
    if (side) { 
        // RIGHT SENSOR
        trigPin = trigPinR;
        echoPin = echoPinR;
    }
    else { 
        // LEFT SENSOR
        trigPin = trigPinL;
        echoPin = echoPinL;
    }

    // secvența inițială
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    // se emite o undă
    digitalWrite(trigPin, LOW);

    // se măsoară durata în care unda este recepționată
    durata = pulseIn(echoPin, HIGH);

    // se calculează distanța
    // (se cunoaște viteza de propagare a undei emise)
    distanta = (durata / 2) / 29.1;

    return distanta;
}

void moveLinear(float distance) {
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  if (distance > 0) {
    go(maxPower,maxPower);
  } else if (distance < 0) {
    go(-maxPower,-maxPower);
    distance = -distance;
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft && !stopRight){
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2) {
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

void rotateDegree(long nr) {
  if (nr < 0) {
    go(-maxPower,maxPower);
    nr = -nr;
    while(left_front_encoder_count < countRotate1Inner*nr && right_front_encoder_count < countRotate1Outer*nr);
  } else if (nr > 0) {
    go(maxPower,-maxPower);
    while(left_front_encoder_count < countRotate1Outer*nr && right_front_encoder_count < countRotate1Inner*nr);
  } else {
    return;
  }
  breakAllEngines();
  delay(100);  
  go(0,0);
}

void stopLeftEngines() {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 4095);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 4095);
}

void stopRightEngines() {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 4095);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 4095);
}
void breakAllEngines() {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 4095);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 4095);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 4095);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, maxPower);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, maxPower);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, maxPower);
}
/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {

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

void followObjectPID(long left, long right)
{
    long avg = (right + left) / 2;
    int P, I, D;
    if (left - right > 10) {
        // turn right
        go(maxPower/2,-maxPower/2);
        delay(delayMove);

        // pentru stabilitate, oprește motoarele
        // la fiecare curbă mică
        go(0,0);
    }
    else if (right - left > 10) {
        // turn left
        go(-maxPower/2,maxPower);
        delay(delayMove);

        // pentru stabilitate, oprește motoarele
        // la fiecare curbă mică
        go(0,0);
    }
    else {
        // PID control
        error = avg - setDistance;
        P = error;

        if (error > 80) {
#ifdef BLE_DEBUG_AUTO_MODE
            BTSerial.println("No object in front of the robot");
#endif
        }
        else {
            I += error;
            if (I > maxPower)
                I = maxPower;
            if (I < -maxPower)
                I = -maxPower;

            D = error - lastError;
            if (D > maxPower)
                D = maxPower;
            if (D < -maxPower)
                D = -maxPower;

            lastError = error;

            int command = Kp * P + Ki * I + Kd * D;

#ifdef BLE_DEBUG_AUTO_MODE    
            BTSerial.print("Comanda: ");
            BTSerial.println(command);
            BTSerial.print("Integrator: ");
            BTSerial.println(I);
            BTSerial.print("Eroare: ");
            BTSerial.println(error);
#endif
            if (command >= 0) {
                if (command > maxPower)
                    command = maxPower;
                go(command,command);
                delay(5);
            }
            if (command < 0) {
                if (command < -maxPower)
                    command = maxPower;
                go(-command, - command);
                delay(5);
            }
            if (abs(error) < 3) {
                // dacă distanța este între 17 și 23
                // atunci oprește motoare
                breakAllEngines();
                delay(10);
                go(0,0);
            }
        }
    }
}
void printMenu() {
    BTSerial.println( "MENU:" );
    BTSerial.println( "a# auto mode");
    BTSerial.println( "s# Stop" );
    BTSerial.println( "f# forward util stop");
    BTSerial.println( "fxx# move forward xx cm");
    BTSerial.println( "b# backward until stop");
    BTSerial.println( "bxx# bacward xx cm");
    BTSerial.println( "l# left until stop");
    BTSerial.println( "rxx# rotate - for left + for right");
    BTSerial.println( "r# right until stop");
    BTSerial.println( "c# print and clear encoders");
    BTSerial.println( "vxx# change maxPower");
    BTSerial.println( "pxx# change Kp");
    BTSerial.println( "ixx# change Ki");
    BTSerial.println( "dxx# change Kd");
    BTSerial.println( "mxx# delay for moving");
    BTSerial.println( "txx# change auto time in s");
}

void setup()
{
    //init the PWM driver
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);    
    //sensors
    pinMode(echoPinL, INPUT);
    pinMode(echoPinR, INPUT); 
    pinMode(trigPinL, OUTPUT);
    pinMode(trigPinR, OUTPUT);
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
//    enableInterrupt(LEFT_BACK_ENCODER_PIN, isrLeftBackEncoder, RISING);
//    enableInterrupt(RIGHT_BACK_ENCODER_PIN, isrRightBackEncoder, RISING);
    printMenu();
    cleanupBT = false;
    //Tune PID
    Kp = 500 ;
    Kd = 0;
    Ki = 0.2;
    delayMove = 50;
    maxPower=4000;
    autoTime = 40L * 1000L;
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
      if (inData[0] == 'a') {
       BTSerial.println("Auto mode");
       dt = millis();

       while ((millis() - dt) < autoTime) {
          long distanceR, distanceL;
          distanceR = read(1);
#ifdef BLE_DEBUG_AUTO_MODE              
          BTSerial.print("Right:  ");
          BTSerial.print(distanceR);
          BTSerial.print(" cm");
#endif                    
          delay(200);
          distanceL = read(0);
#ifdef BLE_DEBUG_AUTO_MODE                    
          BTSerial.print("  Left:  ");
          BTSerial.print(distanceL);
          BTSerial.println(" cm");
#endif
          followObjectPID(distanceL, distanceR);
       }
       dt = millis();
       BTSerial.flush();
       makeCleanup();
       go(0,0);
       isValidInput = true;
      } else if (inData[0] == 'c') {
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
        for (int i = 0 ; i < strlen(inData); i++) {
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
        moveLinear(atof(inData));
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
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
        for (int i = 0 ; i < strlen(inData); i++) {
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
        moveLinear(-atof(inData));
        makeCleanup();
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
      } else if (inData[0] == 'p') {
        //remove p from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change Kp ");
        BTSerial.print(atof(inData));
        printFrom();
        BTSerial.println(Kp);
#endif
        Kp = atof(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'd') {
        //remove d from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change Kd ");
        BTSerial.print(atof(inData));
        printFrom();
        BTSerial.println(Kd);
#endif
        Kd = atof(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'i') {
        //remove i from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change Ki ");
        BTSerial.print(atof(inData));
        printFrom();
        BTSerial.println(Ki);
#endif
        Ki = atof(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'm') {
        //remove m from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE
        BTSerial.print("Change delayMove ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(delayMove);
#endif
        delayMove = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 't') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change auto time ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(autoTime/1000L);
#endif
        autoTime = atol(inData)*1000L;
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
