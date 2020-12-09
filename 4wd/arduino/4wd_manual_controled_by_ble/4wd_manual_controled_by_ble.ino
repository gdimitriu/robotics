/*

Calibration of engines using encoder phase A and L298N micro driver.

Copyright 2020 Gabriel Dimitriu

This file is part of Robotic Calibration

Robotic Calibration is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Robotic Calibration is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Robotic Calibration; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define SERIAL_DEBUG_MODE true
#define BLE_DEBUG_MODE true
#define BLE_COMMAND_MODE true
//#define SERIAL_COMMAND_MODE true

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

#ifdef BLE_DEBUG_MODE
NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}
#endif
Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);


void setup() {
#ifdef SERIAL_DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");  
#endif
  isValidInput = false;
#ifdef BLE_DEBUG_MODE  
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
#endif
  pwmDriver.begin();
  pwmDriver.setOscillatorFrequency(27000000);
  pwmDriver.setPWMFreq(50.0);
  Wire.setClock(400000);
  cleanupBT = false;
#ifdef BLE_COMMAND_MODE  
  BTSerial.println( "-----------------------------------------------------------------------" );
  BTSerial.println( "Calibration of 4wd engines using driver L298N micro and extender PCA9685");
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "MENU:" );
  BTSerial.println( "s# Stop" );
  BTSerial.println( "f# full forward util stop command");
  BTSerial.println( "b# full backward until stop command");
  BTSerial.println( "l# left until stop command");
  BTSerial.println( "r# right until stop command");
  BTSerial.println( "vxx# change speed value");
  BTSerial.println( "-----------------------------------------------------------------------" );
#endif     
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
#ifdef BLE_DEBUG_MODE
    BTSerial.println("Stop all engines");
#endif
#ifdef SERIAL_DEBUG_MODE
    Serial.println("Stop all engines");
#endif
    return;
  }
  if (speedLeft > 0) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
#ifdef BLE_DEBUG_MODE
   BTSerial.print("power left engine=");BTSerial.println(speedLeft);
#endif
#ifdef SERIAL_DEBUG_MODE
    Serial.print("power left engine=");Serial.println(speedLeft);
#endif
  } 
  else {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, -speedLeft);    
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, -speedLeft);    
#ifdef BLE_DEBUG_MODE
    BTSerial.print("power left engine=");BTSerial.println(speedLeft);
#endif
#ifdef SERIAL_DEBUG_MODE
    Serial.print("power left engine=");Serial.println(speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("power right engine=");BTSerial.println(speedRight);
#endif
#ifdef SERIAL_DEBUG_MODE
    Serial.print("power right engine=");Serial.println(speedRight);
#endif
  }else {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);    
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, -speedRight);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, -speedRight);
#ifdef BLE_DEBUG_MODE
    BTSerial.print("power right engine=");BTSerial.println(speedRight);
#endif
#ifdef SERIAL_DEBUG_MODE
    Serial.print("power right engine=");Serial.println( speedRight);
#endif    
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
    if (strcmp(inData,"f") == 0) {
#ifdef SERIAL_DEBUG_MODE      
      Serial.println("forward");
#endif
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("forward");
#endif
      go(maxPower, maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
#ifdef SERIAL_DEBUG_MODE      
      Serial.println("stop");
#endif
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif
      go(0, 0);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"b") == 0) {
#ifdef SERIAL_DEBUG_MODE      
      Serial.println("backward");
#endif
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("backward");
#endif
      go(-maxPower, -maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0 ) {
#ifdef SERIAL_DEBUG_MODE      
      Serial.println("rotate left");
#endif
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate left");
#endif
      go(-maxPower,maxPower);
      makeCleanup();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
#ifdef SERIAL_DEBUG_MODE      
      Serial.println("rotate right");
#endif
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
#ifdef SERIAL_DEBUG_MODE    
          Serial.println("Speed value should be between 0 and 4095");
#endif
#ifdef BLE_DEBUG_MODE    
          BTSerial.println("Speed value should be between 0 and 4095");
#endif
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef SERIAL_DEBUG_MODE        
        Serial.print("Change speed ");
        Serial.print(atol(inData));
        Serial.print(" from ");
        Serial.println(maxPower);
#endif
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(maxPower);
#endif
        maxPower = atol(inData);
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
#ifdef SERIAL_COMMAND_MODE
  Serial.println( "-----------------------------------------------------------------------" );
  Serial.println( "Calibration of 4wd engines using driver L298N micro and extender PCA9685");
  Serial.println( "-----------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "s# Stop" );
  Serial.println( "f# full forward util stop command");
  Serial.println( "b# full backward until stop command");
  Serial.println( "l# left until stop command");
  Serial.println( "r# right until stop command");
  Serial.println( "vxx# change speed value");
  Serial.println( "-----------------------------------------------------------------------" );
   do {
    for (index = 0; index < 20; index++)
    {
       inData[index] = '\0';
    }
    inChar = '0';
    index = 0;
    while(inChar != '#') {
      while( !Serial.available() )
        ; // LOOP...
      while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = Serial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
              inData[index] = '\0'; // Null terminate the string
          }
      }
    }
    if(makeMove() ==false) {
      break;
    }
  } while( isValidInput == true );
#endif
#ifdef BLE_COMMAND_MODE
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
     BTSerial.println( "-----------------------------------------------------------------------" );
     BTSerial.println( "Calibration of 4wd engines using driver L298N micro and extender PCA9685");
     BTSerial.println( "-----------------------------------------------------" );
     BTSerial.println( "MENU:" );
     BTSerial.println( "s# Stop" );
     BTSerial.println( "f# full forward util stop command");
     BTSerial.println( "b# full backward until stop command");
     BTSerial.println( "l# left until stop command");
     BTSerial.println( "r# right until stop command");
     BTSerial.println( "vxx# change speed value");
     BTSerial.println( "-----------------------------------------------------------------------" );
  }
#endif
}
