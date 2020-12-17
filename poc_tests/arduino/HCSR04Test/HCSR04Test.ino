/*
 * HC-SR04 test
 * Copyright 2018 Gabriel Dimitriu

 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
unsigned int EchoPin = 10;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int TrigPin = 11;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned long Len_mm  = 0;
void setup()
{  //Initialize
    Serial.begin(9600);                        //Serial: output result to Serial monitor
    pinMode(EchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(TrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
}

void loop()
{
  /*
    digitalWrite(TrigPin, HIGH);              //begin to send a high pulse, then US-020 begin to measure the distance
    delayMicroseconds(50);                    //set this high pulse width as 50us (>10us)
    digitalWrite(TrigPin, LOW);               //end this high pulse
    
    Time_Echo_us = pulseIn(EchoPin, HIGH);  //calculate the pulse width at EchoPin, 
    if((Time_Echo_us < 60000) && (Time_Echo_us > 1))     //a valid pulse width should be between (1, 60000).
    {
      Len_mm = (Time_Echo_us*34/100)/2;      //calculate the distance by pulse width, Len_mm = (Time_Echo_us * 0.34mm/us) / 2 (mm)
      Serial.print("Present Distance is: ");  //output result to Serial monitor
      Serial.print(Len_mm, DEC);            //output result to Serial monitor
      Serial.println("mm");                 //output result to Serial monitor
    }  */
    Serial.print("distance=");
    Serial.print(calculateDistance(TrigPin, EchoPin));
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
      return ((Time_Echo_us*34/100)/2);
    } else {
      Serial.println("err");
    }
  return 0;
}
