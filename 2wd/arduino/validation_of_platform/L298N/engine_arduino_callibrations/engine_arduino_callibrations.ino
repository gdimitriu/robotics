/*
 * two engine using L298N driver
 * Copyright 2018 Gabriel Dimitriu
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
int LEFT_MOTOR_PIN1 = 6;
int LEFT_MOTOR_PIN2 = 5;
int LEFT_MOTOR_SPEED = 4;
int RIGHT_MOTOR_PIN1 = 7;
int RIGHT_MOTOR_PIN2 = 8;
int RIGHT_MOTOR_SPEED = 9;
bool isValidInput;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  isValidInput = false;
}

void loop() {
  Serial.println( "-----------------------------" );
  Serial.println( "Calibration of engines using driver L298N");
  Serial.println( "-----------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "f) Forward");
  Serial.println( "l) Left" );
  Serial.println( "r) Right" );
  Serial.println( "b) Backward" );
  Serial.println( "s) Stop" );
  Serial.println( "1) Rotate left" );
  Serial.println( "2) Rotate right" );
  Serial.println( "-----------------------------" );
  do
  {
    byte c;
    // get the next character from the serial port
    while( !Serial.available() )
      ; // LOOP...
    c = Serial.read();
    // execute the menu option based on the character recieved
    switch( c )
    {
      case 'f':
        Serial.println("forward");
        go(255,255);
        isValidInput = true;
        break;
       case 'b':
        Serial.println("backward");
        go(-255,-255);
        isValidInput = true;
        break;
      case 'l':
        Serial.println("left engine");
        go(255,0);
        isValidInput = true;
        break;
      case 'r':
        Serial.println("right engine");
        go(0,255);
        isValidInput = true;
        break;
      case 's':
        Serial.println("stop");
        go(0,0);
        isValidInput = true;
        break;
      case '1':
        Serial.println("rotate left");
        go(-255,255);
        isValidInput = true;
        break;
      case '2':
        Serial.println("rotate right");
        go(255,-255);
        isValidInput = true;
        break;
     default:
        // wrong character! display the menu again!
        isValidInput = false;
        break;
    }
    } while( isValidInput == true );
}

void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, LOW);
    return;
  }
  if (speedLeft > 0) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,HIGH);
    analogWrite(LEFT_MOTOR_SPEED, speedLeft);
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,HIGH);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, -speedLeft);
  }
 
  if (speedRight > 0) {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,HIGH);
    analogWrite(RIGHT_MOTOR_SPEED, speedRight);
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, -speedRight);
  }
}
