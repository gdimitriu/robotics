/*
 * First Move auto using arduino shield
 * Copyright 2017 Gabriel Dimitriu
 *
 * This file is part of Robotics project

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
 *Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 */
#define MOTOR2_PIN1 3
#define MOTOR2_PIN2 5
#define MOTOR1_PIN1 6
#define MOTOR1_PIN2 9
#define ECHO_PIN 10
#define TRIG_PIN 11
#define MIN_DISTANCE 200
#define MAX_DISTANCE 400
#define STOP_DISTANCE 100
int goneLeft = 0;

unsigned long calculateDistance(int trigPin, int echoPin) {
  //set the trigPin to LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(50);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds 
  // Calculating the distance 
  unsigned long Time_Echo_us = pulseIn(echoPin, HIGH);  //calculate the pulse width at EchoPin, 
  if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))     //a valid pulse width should be between (1, 60000).
  {
    return (Time_Echo_us * 34 / 100) / 2;
  }
  else {
    Serial.println("err");
    return MAX_DISTANCE;
  }
}

void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0) {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, 0);
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, 0);
    return;
  }
  if (speedLeft > 0) {
    analogWrite(MOTOR1_PIN1, speedLeft);
    analogWrite(MOTOR1_PIN2, 0);
  }
  else {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speedLeft);
  }

  if (speedRight > 0) {
    analogWrite(MOTOR2_PIN1, speedRight);
    analogWrite(MOTOR2_PIN2, 0);
  }
  else {
    analogWrite(MOTOR2_PIN1, 0);
    analogWrite(MOTOR2_PIN2, -speedRight);
  }
}
void setup() {
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  pinMode(MOTOR2_PIN1, OUTPUT);
  pinMode(MOTOR2_PIN2, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  int distance = calculateDistance(TRIG_PIN,ECHO_PIN);
  Serial.println(distance);
  if(distance < MAX_DISTANCE && distance > MIN_DISTANCE) {
    go(255, 0);
    delay(800);
    go(0,0);
    goneLeft = goneLeft + 1;
    Serial.println("left");
  } else if (distance < STOP_DISTANCE){
    go(-255,-255);
    delay(1000);
    Serial.println("stop");
  } else if (distance > MAX_DISTANCE) {
    if (goneLeft > 0) {
      go(0,255);
      delay(800);
      go(0,0);
      goneLeft = goneLeft - 1;
      Serial.println("right");
      return;
    }
    go(255,255);
    delay(100);
    go(0,0);
    Serial.println("fata");
  }
}
