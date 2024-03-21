/*
 * 2 Servo calibration this is useful to put servo at center when the data is attached
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
#include <Servo.h>

#define SERVO_PIN 5
Servo servo;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

void menu() {
  Serial.println( "--------------------------------------------------" );
  Serial.println( "Calibration of two servos");
  Serial.println( "--------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "fxxx# put the first servo at xxx degree");
  Serial.println( "c# center servo");
  Serial.println( "-----------------------------" );
}

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  menu();
}

static void makeCleanup() {
  for (index = 0; index < 20; index++) {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
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

void makeMove(char *data) {
  char *realData = data;
  if (strlen(realData) > 0) {
    realData[strlen(realData) -1] = '\0';
  } else {
    return;
  }
  if (strlen(realData) == 1) {
    if (realData[0] == 'c') {
      Serial.println("Center servo");
      servo.write(90);
    }
  } else if (strlen(realData) > 1) {
      if (realData[0] == 'f') {
        realData++;
        if (!isValidNumber(realData)) {
          makeCleanup();
          return;
        }
        int value = atoi(realData);
        if (value < 0 || value > 180) {
          Serial.println("Value not in 0-180");
          makeCleanup();
          return;
        }
        Serial.print("Move to ");Serial.println(value);
        servo.write(value);
        return;
      }
    }
}

void receiveCommand() {
  while(Serial.available() > 0) {
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
    makeCleanup();
  }
}

void loop() {
  receiveCommand();
}
