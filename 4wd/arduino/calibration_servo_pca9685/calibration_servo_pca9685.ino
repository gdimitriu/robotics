/*
  Calibration of the servo using pca9685 controller
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
#include <Arduino.h>

#define FRONT_SERVO_PIN 11

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
int maxValue = 410;
int minValue = 200;
int centerValue = 310;

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

void printMenu() {
    Serial.println( "MENU:" );
    Serial.println( "l# rotate left with fixed value");
    Serial.println( "r# rotate ritht with fixed value");
    Serial.println( "c# center");
    Serial.println( "rxx# rotate with absolute value");
    Serial.println( "Mxxx# set max value");
    Serial.println( "mxxx# set min value");
    Serial.println( "cxxx# set center value");
    Serial.println( "s# recompute values");
}

void setup()
{
    Serial.begin(9600);
    //init the PWM driver
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);    
    isValidInput = false;
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

void loop()
{
  printMenu();
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
    if (strlen(inData) > 1) {
      if (inData[0] == 'r') {
        //remove l from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Moving with value :");
        Serial.println(atoi(inData));
        pwmDriver.setPWM(FRONT_SERVO_PIN,0,atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'm') {
        //remove m from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting min value from :");
        Serial.print(minValue);
        Serial.print(" to ");
        Serial.println(atoi(inData));
        minValue = atoi(inData);
        isValidInput = true;
      } else if (inData[0] == 'M') {
        //remove M from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting max value from :");
        Serial.print(maxValue);
        Serial.print(" to ");
        Serial.println(atoi(inData));
        maxValue = atoi(inData);
        isValidInput = true;
      } else if (inData[0] == 'c') {
        //remove c from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting center value from :");
        Serial.print(centerValue);
        Serial.print(" to ");
        Serial.println(atoi(inData));
        centerValue = atoi(inData);
        isValidInput = true;
      } 
    } else if (inData[0] == 'r') {
      Serial.print("Moving with value: ");
      Serial.println(minValue);
      pwmDriver.setPWM(FRONT_SERVO_PIN,0,minValue);
      isValidInput = true;
    } else if (inData[0] == 'l') {
      Serial.print("Moving with value: ");
      Serial.println(maxValue);
      pwmDriver.setPWM(FRONT_SERVO_PIN,0,maxValue);
      isValidInput = true;
    } else if (inData[0] == 'c') {
      Serial.print("Moving with value: ");
      Serial.println(centerValue);
      pwmDriver.setPWM(FRONT_SERVO_PIN,0,centerValue);
      isValidInput = true;
    } else if (inData[0] == 's') {
      Serial.print("Recompute values: ");
      Serial.print(" min=");Serial.print(minValue);
      Serial.print(" max=");Serial.print(maxValue);
      centerValue = maxValue/2;
      Serial.print(" center=");Serial.println(centerValue);
      isValidInput = true;
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
