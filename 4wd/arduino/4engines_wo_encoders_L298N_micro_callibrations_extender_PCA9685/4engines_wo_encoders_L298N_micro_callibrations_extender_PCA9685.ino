/*
 * Engine calibration wihtout encoder using two L298N micro drivers and
 * i2c pwm expander PCA9685.
 * Copyright 2020 Gabriel Dimitriu
 *
 * This file is part of Robotics

 * Robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
  
 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
  
 * You should have received a copy of the GNU General Public License
 * along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

int LEFT_FRONT_MOTOR_PIN1 = 2;
int LEFT_FRONT_MOTOR_PIN2 = 3;
int RIGHT_FRONT_MOTOR_PIN1 = 0;
int RIGHT_FRONT_MOTOR_PIN2 = 1;

int LEFT_BACK_MOTOR_PIN1 = 13;
int LEFT_BACK_MOTOR_PIN2 = 12;
int RIGHT_BACK_MOTOR_PIN1 = 15;
int RIGHT_BACK_MOTOR_PIN2 = 14;

#define DEBUG_MODE true

int timing90Left = 500;
int timing90Right = 500;
int maximumPowerLevel = 4000;
int timingForward = 200;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool stopFlag = false;
Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);
void setup() {
  Serial.begin(9600);
  isValidInput =false;
  pwmDriver.begin();
  pwmDriver.setOscillatorFrequency(27000000);
  pwmDriver.setPWMFreq(50.0);
  Wire.setClock(400000);
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

void loop() {
  
  Serial.println( "--------------------------------------------------" );
  Serial.println( "Calibration of engines wihtout encoder using L298N");
  Serial.println( "--------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "f# forward" );
  Serial.println( "b# backward" );
  Serial.println( "l# Rotate 90 left" );
  Serial.println( "r# Rotate 90 right" );
  Serial.println( "tlxx# Set timing for rotate 90 degree left");
  Serial.println( "trxx# Set timing for rotate 90 degree right");
  Serial.println( "tfxx# Set timing for forward");
  Serial.println( "tmxx# Set maximum power level");
  Serial.println( "-----------------------------" );
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
    if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData,"f") == 0) {
      Serial.println("forward");
      go(maximumPowerLevel, maximumPowerLevel);
      delay(timingForward);
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"b") == 0) {
      Serial.println("bacward");
      go(-maximumPowerLevel, -maximumPowerLevel);
      delay(timingForward);
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0) {
      Serial.println("rotate 90 l");
      go(-maximumPowerLevel, maximumPowerLevel);
      delay(timing90Left);
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
      Serial.println("rotate 90 r");
      go(maximumPowerLevel, -maximumPowerLevel);
      delay(timing90Right);
      go(0, 0);
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 't' && inData[1] =='r') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 3)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting timing 90 degree right from=");
        Serial.print(timing90Right);
        Serial.print(" to=");
        Serial.println(atoi(inData));
        timing90Right = atoi(inData);
        isValidInput = true;
      } else if (inData[0] == 't' && inData[1] =='l') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        //remove l from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 3)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting timing 90 degree left from=");
        Serial.print(timing90Left);
        Serial.print(" to=");
        Serial.println(atoi(inData));
        timing90Left = atoi(inData);
        isValidInput = true;
      }  else if (inData[0] == 't' && inData[1] =='f') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 3)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting forward timining from=");
        Serial.print(timingForward);
        Serial.print(" to=");
        Serial.println(atoi(inData));
        timingForward = atoi(inData);
        isValidInput = true;
      }  else if (inData[0] == 't' && inData[1] =='m') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        //remove m from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 3)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting maximum power level from=");
        Serial.print(maximumPowerLevel);
        Serial.print(" to=");
        Serial.println(atoi(inData));
        maximumPowerLevel = atoi(inData);
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
