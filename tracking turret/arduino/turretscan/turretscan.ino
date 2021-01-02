/*
 * First turret scan.
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
#include <Servo.h>
#define DEBUG_SERIAL 1
unsigned int RightEchoPin = 7;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int RightTrigPin = 8;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned int LeftEchoPin = 9;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int LeftTrigPin = 10;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned int horizServoPin = 5;
unsigned int verticalServoPin = 6;
unsigned int laserPin = 4;
Servo horizontalServo;
Servo verticalServo;
boolean hasBeenRun=false;

unsigned long Len_mm  = 0;
void setup()
{  //Initialize
 #ifdef DEBUG_SERIAL
    Serial.begin(38400);                        //Serial: output result to Serial monitor
 #endif
    pinMode(LeftEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(LeftTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    pinMode(RightEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(RightTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    horizontalServo.attach(horizServoPin);
    verticalServo.attach(verticalServoPin);
    pinMode(laserPin, OUTPUT);
    digitalWrite(laserPin, HIGH);    
}

void loop()
{
  if (hasBeenRun == false) {
    for(int i = 90; i >= 0; i=i-5) {
      verticalServo.write(i);
      for(int j = 0; j < 180; j++) {
        uint16_t leftDistance;
        uint16_t rightDistance;
        horizontalServo.write(j);
        leftDistance = calculateDistance(LeftTrigPin, LeftEchoPin);
#ifdef DEBUG_SERIAL
        Serial.print("distanceLeft=");
        Serial.print(leftDistance);
        Serial.println("");
#endif
        rightDistance = calculateDistance(RightTrigPin, RightEchoPin);
#ifdef DEBUG_SERIAL
        Serial.print("distanceRight=");
        Serial.print(rightDistance);
        Serial.println("");
#endif
      }
    }
    hasBeenRun = true;
    digitalWrite(laserPin, LOW);
  }
}

/**
 * calculate the distance to the object.
 * trigPin the trigger pin
 * echoPin the echo pin
 */
int calculateDistance(int trigPin, int echoPin) {
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
    if((Time_Echo_us < 60000) && (Time_Echo_us > 1))     //a valid pulse width should be between (1, 60000).
    {
      return ((Time_Echo_us/2)/29.1)*10;
    } else {
#ifdef DEBUG_SERIAL      
      Serial.println("err");
#endif
    }
  return 0;
}
