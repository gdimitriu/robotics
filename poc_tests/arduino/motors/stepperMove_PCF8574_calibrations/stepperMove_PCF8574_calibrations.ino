/*
 * Stepper move calibration using PCF8574 as extender for Arduino.
 * Copyright 2019 Gabriel Dimitriu
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
#include <Wire.h>
#include<PCF8574.h>
int Pin0 = 0;
int Pin1 = 1;
int Pin2 = 2;
int Pin3 = 3;
PCF8574 expander;

static char inData[20]; // Allocate some space for the string
static char inChar; // Where to store the character read
static byte index = 0; // Index into array; where to store the character

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  expander.begin(0x20);
  expander.pinMode(0, OUTPUT);
  expander.pinMode(1, OUTPUT);
  expander.pinMode(2, OUTPUT);
  expander.pinMode(3, OUTPUT);
}
void stepper(int steps) {
  boolean forward = true;
  int step = 0; 
  int nrSteps = steps;
  if (steps < 0) {
    forward = false;
    nrSteps = -steps;
  }
  for (int i = 0 ; i <  nrSteps; i++) {
    switch (step%8) {
    case 0:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, HIGH);
      break;
    case 1:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, HIGH);
      break;
    case 2:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 3:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 4:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 5:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 6:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 7:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, HIGH);
      break;
    default:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    }
    if(forward == true){ step++;}
    if(forward == false){ step--; }
    if(step>7){step=0;}
    if(step<0){step=7;}
    delay(1);
  }
}

static boolean isValidNumber(char *data)
{
  if (strlen(data) == 0 ) return false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<strlen(data);i++) {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

static void makeMove(char *data) {
  char *realData = data;
  if (strlen(realData) > 0) {
    realData[strlen(realData) -1] = '\0';
  } else {
     makeCleanup();
    return;
  }
  if(realData[0] == 'm') {
    realData++;
    if (!isValidNumber(realData)) {
      return;
    }
    stepper(atoi(realData));
  } else if (realData[0] == 's') {
    expander.digitalWrite(Pin0, HIGH);
    expander.digitalWrite(Pin1, HIGH);
    expander.digitalWrite(Pin2, HIGH);
    expander.digitalWrite(Pin3, HIGH);
  } else if (realData[0] == 'S') {
    expander.digitalWrite(Pin0, LOW);
    expander.digitalWrite(Pin1, LOW);
    expander.digitalWrite(Pin2, LOW);
    expander.digitalWrite(Pin3, LOW);
  }
   makeCleanup();
}
static void makeCleanup() {
  for (index = 0; index < 20; index++) {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void receiveCommand() {
  while(Serial.available() > 0) // Don't read unless there you know there is data
  {
     if(index < 19) // One less than the size of the array
     {
        inChar = Serial.read(); // Read a character
        if (inChar == '\r' || inChar == '\n' || inChar == '\0' || inChar < 35 || inChar > 122) {
          continue;
        }
        //commands start with a letter capital or small
        if (index == 0 && !((inChar > 64 && inChar < 91) || (inChar > 96 && inChar < 123))) {
          continue;
        }
        inData[index++] = inChar; // Store it
        inData[index] = '\0'; // Null terminate the string
     } else {
        makeCleanup();
     }
   }
 if (inData[index-1] == '#') {
    makeMove(inData);
 }
}

void loop() {
  receiveCommand();
}
