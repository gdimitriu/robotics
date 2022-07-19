/*
 * Engines with MOSFET type N 30N06L on quadcopter
 * Copyright 2020 Gabriel Dimitriu
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
/*
 * MOS-FET driver configurations
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

#define RIGHT_ENGINE_PIN 5 //grey
#define LEFT_ENGINE_PIN 6 //red
#define FRONT_ENGINE_PIN 10 //orange
#define BACK_ENGINE_PIN 11 //yellow

#define FRONT_ENGINE_IDX 0
#define RIGHT_ENGINE_IDX 1
#define BACK_ENGINE_IDX 2
#define LEFT_ENGINE_IDX 3

int enginePower[4];
void setup() {
  Serial.begin(9600);
  pinMode(FRONT_ENGINE_PIN, OUTPUT);
  pinMode(RIGHT_ENGINE_PIN, OUTPUT);
  pinMode(BACK_ENGINE_PIN, OUTPUT);
  pinMode(LEFT_ENGINE_PIN, OUTPUT);  
  enginePower[FRONT_ENGINE_IDX]=0;
  enginePower[RIGHT_ENGINE_IDX]=0;
  enginePower[BACK_ENGINE_IDX]=0;
  enginePower[LEFT_ENGINE_IDX]=0;
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

void powerToAllEngines(int value) {
  Serial.print(" Power all engines to value:");
  Serial.println(value);
  analogWrite(FRONT_ENGINE_PIN, value);
  analogWrite(RIGHT_ENGINE_PIN, value);
  analogWrite(BACK_ENGINE_PIN, value);
  analogWrite(LEFT_ENGINE_PIN, value);
  enginePower[FRONT_ENGINE_IDX]=value;
  enginePower[RIGHT_ENGINE_IDX]=value;
  enginePower[BACK_ENGINE_IDX]=value;
  enginePower[LEFT_ENGINE_IDX]=value;
}

void powerToFrontEngine(int value) {
  Serial.print("Power to front engine from : ");
  Serial.print(enginePower[FRONT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  analogWrite(FRONT_ENGINE_PIN, value);
  enginePower[FRONT_ENGINE_IDX] = value;
}

void powerToRightEngine(int value) {
  Serial.print("Power to right engine from : ");
  Serial.print(enginePower[RIGHT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  analogWrite(RIGHT_ENGINE_PIN, value);
  enginePower[RIGHT_ENGINE_IDX] = value;
}

void powerToBackEngine(int value) {
  Serial.print("Power to back engine from : ");
  Serial.print(enginePower[BACK_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  analogWrite(BACK_ENGINE_PIN, value);
  enginePower[BACK_ENGINE_IDX] = value;
}

void powerToLeftEngine(int value) {
  Serial.print("Power to left engine from : ");
  Serial.print(enginePower[LEFT_ENGINE_IDX]);
  Serial.print(" to : ");
  Serial.println(value);
  analogWrite(LEFT_ENGINE_PIN, value);
  enginePower[LEFT_ENGINE_IDX] = value;
}

void printAllEnginesLevel() {
  Serial.print("Front engine power level : "); Serial.println(enginePower[FRONT_ENGINE_IDX]);
  Serial.print("Right engine power level : "); Serial.println(enginePower[RIGHT_ENGINE_IDX]);
  Serial.print("Back engine power level : "); Serial.println(enginePower[BACK_ENGINE_IDX]);
  Serial.print("Left engine power level : "); Serial.println(enginePower[LEFT_ENGINE_IDX]);
}

void loop() {
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "Engines with MOS-FET on quadcopter");
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "s# Stop" );
  Serial.println( "axx# Power to all engines" );
  Serial.println( "fxx# Power to front engine" );
  Serial.println( "rxx# Power to right engine" );
  Serial.println( "bxx# Power to back engine" );
  Serial.println( "lxx# Power to left engine" );
  Serial.println( "p# print all engines power level" );
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
    if (strcmp(inData,"s") == 0) {
      Serial.println("stop");
      powerToAllEngines(0);
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0) {
      printAllEnginesLevel();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'a') {
        //remove a from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToAllEngines(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToFrontEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToRightEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToBackEngine(atoi(inData));
        isValidInput = true;
      } else if (inData[0] == 'l') {
        //remove l from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        powerToLeftEngine(atoi(inData));
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
        isValidInput = false;
    }
  } while( isValidInput == true );

}
