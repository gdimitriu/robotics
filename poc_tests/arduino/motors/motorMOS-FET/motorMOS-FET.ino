/*
Engine with MOSFET type N 30N06L
Copyright 2020 Gabriel Dimitriu

This file is part of Robotics.

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
/*
 * Gate to 10 k rezistor and to PWM pin
 * 10k rezistor to GND
 * drain to engine -
 * source to GND
 * engine + to VCC
 */
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
#define MOTOR_PIN 5
void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);
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

void loop() {
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "Engine with MOS-FET");
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "s# Stop" );
  Serial.println( "pxx# Power");
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
    if (inData[0] =='s') {
      Serial.println("stop");
      analogWrite(MOTOR_PIN, 0);
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'p') {
        //remove p from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("power ");
        Serial.println(atoi(inData));
        analogWrite(MOTOR_PIN, atoi(inData));
        isValidInput = true;
      }
    } else {
        isValidInput = false;
    }
  } while( isValidInput == true );
}
