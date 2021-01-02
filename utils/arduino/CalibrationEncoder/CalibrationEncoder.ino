/*
 * Encoder readings calibration.
 * Copyright 2020 Gabriel Dimitriu
 * 
 * This file is part of Robotics.

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
 *Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 */

#include <EnableInterrupt.h>

#define ENCODER_PIN 7

volatile long counter = 0;
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

void isrEncoder() {
  counter++;
}

void setup() {
 Serial.begin(9600);
 isValidInput = false;
 pinMode(ENCODER_PIN, INPUT_PULLUP);
 enableInterrupt(ENCODER_PIN, isrEncoder, RISING);
}


void loop() {
  Serial.println( "-----------------------------" );
  Serial.println( "Encoder calibration");
  Serial.println( "-----------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "p# print encoder counting");
  Serial.println( "c# reset counters");
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
    if (strcmp(inData,"c") == 0) {
      Serial.println("reset counters");
      counter=0;
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0 ) {
        Serial.print("EncoderValues "); Serial.println(counter);
        isValidInput = true;
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}
