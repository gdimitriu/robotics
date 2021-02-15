/*
 * PIXY2 servo calibration on I2C.
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
#include <Pixy2I2C.h>

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
unsigned long hValue;
unsigned long vValue;
int lamp;

Pixy2I2C pixy;
void printMenu() {
    Serial.println( "MENU:" );
    Serial.println( "c# center");
    Serial.println( "vxxx# rotate vertical with absolute value");
    Serial.println( "hxxx# rotate horizontal with absolute value");
    Serial.println("l# turnon or off lamp");
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

void setup() 
{
  Serial.begin(115200);
  Serial.print("Starting...\n");
  Wire.setClock(400000); 
  pixy.init();
  isValidInput = false;
  printMenu();
  lamp = 0;
  // Turn off both lamps, upper and lower for maximum exposure
  if (pixy.setLamp(lamp, lamp) < 0) {
    Serial.println("Could not set lamp");
  }
  // change to the line_tracking program.  Note, changeProg can use partial strings, so for example,
  // you can change to the line_tracking program by calling changeProg("line") instead of the whole
  // string changeProg("line_tracking")
  //pixy.changeProg("line");
  hValue = 500;
  vValue = 500;
  pixy.setServos(hValue,vValue);
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
      if (inData[0] == 'h') {
        //remove h from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Moving horizontally at ");
        Serial.println(atol(inData));
        hValue = atol(inData);
        pixy.setServos(hValue,vValue);
        isValidInput = true;
      } else if (inData[0] == 'v') {
        //remove m from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Moving vertically at ");
        Serial.println(atol(inData));
        vValue = atol(inData);
        pixy.setServos(hValue,vValue);
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else if (inData[0] == 'c') {
        Serial.println("Moving to center");
        hValue = 500;
        vValue = 500;
        pixy.setServos(hValue,vValue);
        isValidInput = true;
    } else if (inData[0] == 'l') {
        Serial.println("turn on / off lamp");
        if (lamp == 0) {
          lamp = 1;
        } else {
          lamp = 0;
        }
        pixy.setLamp(lamp,lamp);
        isValidInput = true;
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
