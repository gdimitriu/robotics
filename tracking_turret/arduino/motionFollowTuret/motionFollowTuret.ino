/*
 * Turret motion follow
 * Copyright 2023 Gabriel Dimitriu
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
#include <Servo.h>
#define SERIAL_DEBUG 1
#define SERIAL_COMMAND 1
unsigned int RightEchoPin = 6;
unsigned int RightTrigPin = 7;
unsigned int LeftEchoPin = 8;
unsigned int LeftTrigPin = 9;
unsigned int horizServoPin = 5;
unsigned int verticalServoPin = 4;

Servo horizontalServo;
Servo verticalServo;

unsigned long LDistance, RDistance;

unsigned long threshold = 500; //mm

unsigned int distanceDiff = 20; //mm

unsigned int angleMove = 2;

int angle = 90; //initial angle

#define SERIAL_BUFFER 50

boolean cleanupSerial;
bool isValidInput;
char inData[SERIAL_BUFFER]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

void setup() {  //Initialize
 #if defined(SERIAL_DEBUG) || defined(SERIAL_COMMAND)
    Serial.begin(38400);
    Serial.println("Starting .... ");
 #endif
    pinMode(LeftEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(LeftTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    pinMode(RightEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(RightTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    horizontalServo.attach(horizServoPin);
    verticalServo.attach(verticalServoPin);
    verticalServo.write(90);
    isValidInput = false;
    cleanupSerial = false;
}


boolean isValidNumber( char *data, int size )
{
  if ( size == 0 ) return false;
   boolean isNum=false;
   if( !( data[0] == '+' || data[0] == '-' || isDigit(data[0]) ) ) return false;

   for( byte i=1; i<size; i++ )
   {
       if( !( isDigit(data[i]) || data[i] == '.' ) ) return false;
   }
   return true;
}

boolean makeCleanup() {
  for ( index = 0; index < SERIAL_BUFFER; index++ ) {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
  Serial.flush();
}

/**
 * calculate the distance to the object.
 * trigPin the trigger pin
 * echoPin the echo pin
 */
unsigned long calculateDistance(int trigPin, int echoPin) {
  //set the trigPin to LOW
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  // Sets the trigPin on HIGH state for 10 micro seconds 
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 
  // Reads the echoPin, returns the sound wave travel time in microseconds 
  // Calculating the distance 
  unsigned long Time_Echo_us = pulseIn(echoPin, HIGH);  //calculate the pulse width at EchoPin, 
  if((Time_Echo_us < 26190) && (Time_Echo_us > 1))     //a valid pulse width should be between (1, 60000).
  {
    return ( ( Time_Echo_us / 2.0 ) / 29.1 ) * 10.0;
  } else {
#ifdef SERIAL_DEBUG
    Serial.print(trigPin);
    Serial.print(" echo=");
    Serial.print(echoPin);      
    Serial.println(" err");
#endif
  }
  return 2500;
}

void follow()  {
  if ( LDistance <= threshold || RDistance <= threshold ) {
    if ( LDistance + distanceDiff <= RDistance ) {
      angle = angle - angleMove;
    } else if ( RDistance + distanceDiff <= LDistance ) {
      angle = angle + angleMove;
    }
  }
  if ( angle > 160 ) {
    angle = 160;
  }
  if ( angle < 0 ) {
    angle = 0;
  }
  horizontalServo.write(angle);
}

void makeSetup() {
  if ( strlen(inData) <=2 ) {
    switch ( inData[0] ) {
      case 't':
        Serial.println(threshold);
        break;
      case 'd':
        Serial.println(distanceDiff);
        break;
      case 'a':
        Serial.println(angleMove);
        break;
      default:
        Serial.println("unknown command");
        isValidInput = false;
        makeCleanup();
        return false;
    }
  } else if ( strlen(inData) > 2 ) {    
    if ( inData[0] == 't' ) {
      //remove #
      if ( index > 0 ) {
        inData[index-1] = '\0';
      }
      //remove t from command
      for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
        inData[i]=inData[i+1];
      }
      if ( !isValidNumber(inData, index - 2) ) {
        Serial.println("unknown command");
        isValidInput = false;
        makeCleanup();
        return false;
      }
      unsigned long thresholdOld = threshold;
      threshold = atol(inData);
      Serial.println(thresholdOld);
    } else if ( inData[0] == 'a' ) {
      //remove #
      if ( index > 0 ) {
        inData[index-1] = '\0';
      }
      //remove a from command
      for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
        inData[i]=inData[i+1];
      }
      if ( !isValidNumber(inData, index - 2) ) {
        Serial.println("unknown command");
        isValidInput = false;
        makeCleanup();
        return false;
      }
      unsigned long angleMoveOld = angleMove;
      angleMove = atol(inData);
      Serial.println(angleMoveOld);
    } else if ( inData[0] == 'd' ) {
      //remove #
      if ( index > 0 ) {
        inData[index-1] = '\0';
      }
      //remove d from command
      for ( uint8_t i = 0 ; i < strlen(inData); i++ ) {
        inData[i]=inData[i+1];
      }
      if ( !isValidNumber(inData, index - 2) ) {
        Serial.println("unknown command");
        isValidInput = false;
        makeCleanup();
        return false;
      }
      unsigned long distanceDiffOld = distanceDiff;
      distanceDiff = atol(inData);
      Serial.println(distanceDiffOld);
    } else {
      Serial.println("unknown command");
      isValidInput = false;
      makeCleanup();
      return false;
    }
  }
  isValidInput = true;
  makeCleanup();
  return true;
}

void loop() {
  #ifdef SERIAL_COMMAND
    while ( Serial.available() > 0 ) // Don't read unless there you know there is data
    {
      if ( index < (SERIAL_BUFFER - 1) ) // One less than the size of the array
      {
          inChar = Serial.read(); // Read a character
          if ( inChar=='\r' || inChar=='\n' || inChar =='\0' || inChar < 35 || inChar > 122 ) {
            continue;
          }
          //commands start with a letter capital or small
          if ( index == 0 && !( ( inChar >64 && inChar <91 ) || ( inChar > 96 && inChar<123 ) ) ) {
            continue;
          }    
          inData[index++] = inChar; // Store it
          inData[index] = '\0'; // Null terminate the string
          if ( inChar == '#' ) {
            break;
          }
      } else {
          makeCleanup();
          cleanupSerial = true;
      }
  }
  if ( index >= 1 ) {
    if ( inData[index - 1] == '#' ) {
      makeSetup();
    } else if ( cleanupSerial ) {
      makeCleanup();
      cleanupSerial = false;
    } else {
      delay(10);
    }
  }
  #endif
  LDistance = calculateDistance(LeftTrigPin, LeftEchoPin);
  RDistance = calculateDistance(RightTrigPin, RightEchoPin);
  follow();
}
