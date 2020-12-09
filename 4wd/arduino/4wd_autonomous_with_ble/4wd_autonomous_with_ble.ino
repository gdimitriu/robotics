/*
  4 whell autonomous droid using bluetooth.
  this is my version of the droid from "Introducere in Arduino"
  by Optimnus Digital
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
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 3

#define BLE_DEBUG_MODE true

#define trigPinR 5
#define echoPinR 6
#define trigPinL 10
#define echoPinL 11

#define setDistance 20

int LEFT_FRONT_MOTOR_PIN1 = 2;
int LEFT_FRONT_MOTOR_PIN2 = 3;
int RIGHT_FRONT_MOTOR_PIN1 = 0;
int RIGHT_FRONT_MOTOR_PIN2 = 1;

int LEFT_BACK_MOTOR_PIN1 = 13;
int LEFT_BACK_MOTOR_PIN2 = 12;
int RIGHT_BACK_MOTOR_PIN1 = 15;
int RIGHT_BACK_MOTOR_PIN2 = 14;
unsigned long autoTime;
#  define second 1000

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

// reține timpul de rulare a programului
long dt;

int turatie; // controlează turația motoarelor
bool sens;   // controlează sensul motoarelor
int btCommand;
int mode = NULL;
bool lastDirection = true;

// variables used for PID algorithm
int maxPower;
int lastError = 0;
int error;
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

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
/*
 * 
 #ifdef BLE_DEBUG_MODE
    BTSerial.print("left=");BTSerial.print(speedLeft);
    BTSerial.print(" right=");BTSerial.println(speedRight);
#endif
*/
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
#ifdef BLE_DEBUG_MODE
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

#ifdef BLE_DEBUG_MODE    
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
                go(0,0);
            }
        }
    }
}
void printMenu() {
    BTSerial.println( "-----------------------------------------------------------------------" );
    BTSerial.println( "Calibration of 4wd engines using driver L298N micro and extender PCA9685");
    BTSerial.println( "-----------------------------------------------------" );
    BTSerial.println( "MENU:" );
    BTSerial.println( "a# autonomous mode");
    BTSerial.println( "s# Stop" );
    BTSerial.println( "f# full forward util stop command");
    BTSerial.println( "b# full backward until stop command");
    BTSerial.println( "l# left until stop command");
    BTSerial.println( "r# right until stop command");
    BTSerial.println( "vxx# change speed value");
    BTSerial.println( "pxx# proportional constant");
    BTSerial.println( "ixx# integration constant");
    BTSerial.println( "dxx# derivative constant");
    BTSerial.println( "mxx# delay for moving constant");
    BTSerial.println( "txx# changing auto time in seconds");
    BTSerial.println( "-----------------------------------------------------------------------" );

}
void setup()
{
    //init the PWM driver
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);
    BTSerial.begin(38400);
    pinMode(echoPinL, INPUT); // undă recepționată
    pinMode(echoPinR, INPUT); // undă recepționată
    pinMode(trigPinL, OUTPUT); // undă emisă
    pinMode(trigPinR, OUTPUT); // undă emisă    
    enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
    BTSerial.println("Starting...");
    printMenu();
    cleanupBT = false;
    //Tune PID
    Kp = 500 ;
    Kd = 0;
    Ki = 0.2;
    delayMove = 50;
    maxPower=4000;
    autoTime = 40L * (long)second;
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

boolean makeMove() {
     if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData, "a") == 0) {
       BTSerial.println("Autonomous mode");
       dt = millis();

       while ((millis() - dt) < autoTime) {
          long distanceR, distanceL;
          distanceR = read(1);
#ifdef BLE_DEBUG_MODE              
          BTSerial.print("Right:  ");
          BTSerial.print(distanceR);
          BTSerial.print(" cm");
#endif                    
          delay(200);
          distanceL = read(0);
#ifdef BLE_DEBUG_MODE                    
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
    } else if (strcmp(inData,"f") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("forward");
#endif
      go(maxPower, maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif
      go(0, 0);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"b") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("backward");
#endif
      go(-maxPower, -maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0 ) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate left");
#endif
      go(-maxPower,maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate right");
#endif
      go(maxPower,-maxPower);
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
#ifdef BLE_DEBUG_MODE    
          BTSerial.println("Speed value should be between 0 and 4095");
#endif
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change Kp ");
        BTSerial.print(atof(inData));
        BTSerial.print(" from ");
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change Kd ");
        BTSerial.print(atof(inData));
        BTSerial.print(" from ");
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change Ki ");
        BTSerial.print(atof(inData));
        BTSerial.print(" from ");
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change delayMove ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
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
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change autonomous time ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(autoTime/second);
#endif
        autoTime = atol(inData)*second;
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
