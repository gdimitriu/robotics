/*
  2 Servo calibration this is useful to put servo at center when the data is attached
  Copyright 2020 Gabriel Dimitriu

  This is part of Robotics
  
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
#include <Servo.h>

#define SERVO1_PIN 4
#define SERVO2_PIN 7
Servo firstServo;
Servo secondServo;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;

boolean isValidNumber(char *data, int size)
{
  if (size == 0 ) return false;
   boolean isNum=false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++)
   {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   if (atoi(data) > 180) return false;
   if (atoi(data) < 0) return false;
   return true;
}

void setup() {
  Serial.begin(9600);
  isValidInput = false;
  firstServo.attach(SERVO1_PIN);
  secondServo.attach(SERVO2_PIN);
  firstServo.write(90);
  secondServo.write(90);
}

void loop() {
  Serial.println( "--------------------------------------------------" );
  Serial.println( "Calibration of two servos");
  Serial.println( "--------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "fxxx# put the first servo at xxx degree");
  Serial.println( "sxxx# put the second servo at xxx degree");
  Serial.println( "c# center all servo");
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
    if (strlen(inData) == 1) {
      if (inData[0] == 'c') {
        Serial.println("Center all servos");
        firstServo.write(90);
        secondServo.write(90);
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        firstServo.write(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 's') {
        //remove s from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        secondServo.write(atoi(inData));
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
