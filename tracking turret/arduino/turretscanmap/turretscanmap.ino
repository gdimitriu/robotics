/*
 * First turret scan and mapping using spi flash.
 * Copyright 2021 Gabriel Dimitriu
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
#include <SerialFlash.h>
#include <SPI.h>
#include <NewPing.h>

//#define DEBUG_SERIAL 1
unsigned int RightEchoPin = 6;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int RightTrigPin = 7;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned int LeftEchoPin = 8;           // connect Pin 2(Arduino digital io) to Echo at US-020
unsigned int LeftTrigPin = 9;           // connect Pin 3(Arduino digital io) to Trig at US-020
unsigned int horizServoPin = 5;
unsigned int verticalServoPin = 4;
unsigned int laserPin = 3;
Servo horizontalServo;
Servo verticalServo;
boolean hasBeenRun=false;
boolean seekEnemy = false;
#define CSPIPIN 10
#define ERROR_MARGIN 10
SerialFlashFile file;
unsigned long Len_mm  = 0;
NewPing rightSensor(RightTrigPin,RightEchoPin,4000);
void setup()
{  //Initialize
// #ifdef DEBUG_SERIAL
    Serial.begin(38400);                        //Serial: output result to Serial monitor
// #endif
    pinMode(LeftEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(LeftTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    pinMode(RightEchoPin, INPUT);                    //Set EchoPin as input, to receive measure result from US-020
    pinMode(RightTrigPin, OUTPUT);                   //Set TrigPin as output, used to send high pusle to trig measurement (>10us)
    horizontalServo.attach(horizServoPin);
    verticalServo.attach(verticalServoPin);
    pinMode(laserPin, OUTPUT);
    digitalWrite(laserPin, LOW);
    if (!SerialFlash.begin(CSPIPIN)) {
        while(1) {
          Serial.println("Unable to access SPI Flash chip");
          delay(1000);
        }
    }
    if (SerialFlash.exists("map.dat")) {
      file = SerialFlash.open("map.dat");
      file.seek(0);
//      file.erase();
      hasBeenRun = true;
    } else {
      SerialFlash.createErasable("map.dat",2520);
      file = SerialFlash.open("map.dat");
      file.seek(0);
      hasBeenRun = false;
    }
}

void loop()
{
  if (hasBeenRun == false) {
    for(int i = 90; i <= 160; i=i+5) {
      verticalServo.write(i);
      for(int j = 0; j < 180; j=j+2) {
        uint16_t rightDistance;
        horizontalServo.write(j);
        rightDistance = NewPing::convert_cm(rightSensor.ping_median(3));
//        rightDistance = rightSensor.ping_cm();
#ifdef DEBUG_SERIAL
        Serial.print("distanceRight=");
        Serial.print(rightDistance);
        Serial.println("");
#endif
        file.write(&rightDistance,2);
        file.flush();
      }
    }
    hasBeenRun = true;
    digitalWrite(laserPin, LOW);
  } else if(seekEnemy == false) {
    file.seek(0);
    for(int i = 90; i <= 160; i=i+5) {
      verticalServo.write(i);
      for(int j = 0; j < 180; j=j+2) {
        uint16_t rightDistance;
        horizontalServo.write(j);
//        rightDistance = NewPing::convert_cm(rightSensor.ping_median(3));
        rightDistance = rightSensor.ping_cm();
        uint16_t mapDistance = 0;
        file.read(&mapDistance,2);
        int16_t diff = mapDistance-rightDistance;
        if (abs(diff)>ERROR_MARGIN) {
          digitalWrite(laserPin,HIGH);
          Serial.print("Found enemy at ");Serial.print(rightDistance);Serial.print(" vs ");Serial.print(mapDistance);Serial.print(" and diff =");Serial.print(diff);
          Serial.print(" at ");Serial.print(i);Serial.print(" ");Serial.println(j);
          //delay(1000);
          digitalWrite(laserPin,LOW);
        } else {
          Serial.print(" at ");Serial.print(i);Serial.print(" ");Serial.println(j);
        }
      }
    }
    seekEnemy = true;
  }
}
