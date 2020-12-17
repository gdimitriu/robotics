/*
 * HC-SR04 on servo on the droid.
 * Copyright (C) 2018 Gabriel Dimitriu
 * All rights reserved.
 *
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
 */#include <AutonomousDroid.h>
#include <Servo.h>

#define SERVO_PIN 12
#define ECHO_PIN 10
#define TRIG_PIN 11

#define MIN_DISTANCE 100 //for testing the radar
#define MAX_DISTANCE 4000
#define STOP_DISTANCE 100

//servo instance
Servo senzorServo;

AutonomousDroid droid = AutonomousDroid();

int pos = 90;
void setup() {
  Serial.begin(9600);
  senzorServo.attach(SERVO_PIN);
  senzorServo.write(90);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
}

int look(int position) {
  senzorServo.write(position);
  //wait to position the servo
  delay(700);
  int distance = droid.calculateDistance(TRIG_PIN,ECHO_PIN);
  Serial.print("look at position:"); Serial.print(position);
  Serial.print(" has distance:"); Serial.println(distance);
  return distance;
}

void loop() {
  int distance = droid.calculateDistance(TRIG_PIN,ECHO_PIN);
  Serial.println(distance);
  if (distance > MIN_DISTANCE) {
    Serial.println("move forward");
  } else {
    //look left
    int leftDistance = look(180);
    //look right
    int rightDistance = look(0);
    //reset the senzorServor
    senzorServo.write(90);
    if (leftDistance < rightDistance) {
      Serial.println("move right");
    } else if (leftDistance > rightDistance) {
      Serial.println("move left");
    } else {
      Serial.println("move back");
    }
    delay (100);
  }
  
}
