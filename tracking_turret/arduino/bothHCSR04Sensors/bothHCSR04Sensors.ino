/*
 * test both HCSR04 sensors on tracking turret
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
unsigned int RightEchoPin = 7;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int RightTrigPin = 8;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned int LeftEchoPin = 9;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int LeftTrigPin = 10;           // connect Pin 3(Arduino digital io) to Trig at US-020

unsigned long Len_mm  = 0;
void setup()
{  //Initialize
    Serial.begin(9600);                        //Serial: output result to Serial monitor
    pinMode(LeftEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(LeftTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    pinMode(RightEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(RightTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
}

void loop()
{
    Serial.print("distanceLeft=");
    Serial.print(calculateDistance(LeftTrigPin, LeftEchoPin));
    Serial.println("");
    delay(50);
    Serial.print("distanceRight=");
    Serial.print(calculateDistance(RightTrigPin, RightEchoPin));
    Serial.println("");
    delay(1000);                            //take a measurement every second (1000ms)
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
      Serial.println("err");
    }
  return 0;
}
