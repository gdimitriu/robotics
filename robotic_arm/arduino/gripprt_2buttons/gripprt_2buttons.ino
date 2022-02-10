/*
 * gripper 2 buttons.
 * Copyright 2022 Gabriel Dimitriu
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

#define SERVO_PIN 12
#define CLOSE_PIN 2
#define OPEN_PIN 3
//servo instance
Servo myServo;
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
int openValue = 100;
int closeValue = 15;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(openValue);
  pinMode(OPEN_PIN, INPUT);
  pinMode(CLOSE_PIN, INPUT);
}

void loop()
{
  int openPin = digitalRead(OPEN_PIN);
  int closePin = digitalRead(CLOSE_PIN);
  if (openPin == 0 && closePin == 1) {
    Serial.println("Open");
    myServo.write(openValue);
  } else if (closePin == 0 && openPin == 1) {
    Serial.println("Close");
    myServo.write(closeValue);
  }
}
