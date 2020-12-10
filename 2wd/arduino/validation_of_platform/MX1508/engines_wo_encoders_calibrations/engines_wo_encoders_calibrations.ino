/*
  Engine calibration wihtout encoder using driver MX1508.
  Copyright 2019 Gabriel Dimitriu

  This file is part of Robotics

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

int LEFT_MOTOR_PIN1 = 5;
int LEFT_MOTOR_PIN2 = 6;
int RIGHT_MOTOR_PIN1 = 9;
int RIGHT_MOTOR_PIN2 = 10;

#define DEBUG_MODE true

int timing90Left = 1900;
int timing90Right = 1900;
int timing1Degree = 100;
int maximumPowerLevel = 255;
int timingForward = 1000;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool stopFlag = false;

void setup() {
  Serial.begin(9600);
  isValidInput =false;
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
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
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
  }
}
/*
 * turn the platform xx degree
 */
void turnNrDegree(int degree)
{
  if (degree < 0) {
    turnNrDegreeLeft(-degree);
  }
  else {
    turnNrDegreeRight(degree);
  }
}
/*
 * turn the platform xx degree left
 */
void turnNrDegreeLeft(int degree)
{
    go(-maximumPowerLevel, maximumPowerLevel);
    for (int i = 0; i < degree; i++) {
      delay(timing1Degree);
    }
    go(0, 0);
}

/*
 * turn the platform xx degree right
 */
void turnNrDegreeRight(int degree)
{
    go(maximumPowerLevel, -maximumPowerLevel);
    for (int i = 0; i < degree; i++) {
      delay(timing1Degree);
    }
    go(0, 0);
}

void loop() {
  
  Serial.println( "--------------------------------------------------" );
  Serial.println( "Calibration of engines wihtout encoder using MX1508");
  Serial.println( "--------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "f# forward" );
  Serial.println( "b# backward" );
  Serial.println( "l# Rotate 90 left" );
  Serial.println( "r# Rotate 90 right" );
  Serial.println( "xx# Rotate xx degree");
  Serial.println( "tdxx# Set timing for rotate 1 degree");
  Serial.println( "tlxx# Set timing for rotate 90 degree left");
  Serial.println( "trxx# Set timing for rotate 90 degree right");
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
    } else if (isValidNumber(inData, index - 1)){
      Serial.println("rotate degree");
      turnNrDegree(atol(inData));
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 't' && inData[1] =='d') {
        //remove t from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        //remove d from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 3)) {
          isValidInput = false;
          break;
        }
        Serial.print("Setting timing 1 degree from=");
        Serial.print(timing1Degree);
        Serial.print(" to=");
        Serial.println(atoi(inData));
        timing1Degree = atoi(inData);
        isValidInput = true;
      } else if (inData[0] == 't' && inData[1] =='r') {
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
