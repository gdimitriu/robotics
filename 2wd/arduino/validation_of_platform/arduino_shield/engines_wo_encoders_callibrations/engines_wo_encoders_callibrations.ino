/*
Engines without encoders using Arduino Shield L298 V2
Copyright 2018 Gabriel Dimitriu

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
int LEFT_MOTOR_PIN1 = 3;
int LEFT_MOTOR_PIN2 = 5;
int RIGHT_MOTOR_PIN1 = 6;
int RIGHT_MOTOR_PIN2 = 9;
bool isValidInput;

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  isValidInput = false;
}

void loop() {
  Serial.println( "-----------------------------" );
  Serial.println( "Calibration of engines using arduino shield L298 V2");
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
    Serial.print( "?" );
    while( !Serial.available() )
      ; // LOOP...
    c = Serial.read();
    // execute the menu option based on the character recieved
    switch( c )
    {
      case 'f':
        go(255,255);
        isValidInput = true;
        break;
      case 'b':
        go(-255,-255);
        isValidInput = true;
        break;
      case 'l':
        go(255,0);
        isValidInput = true;
        break;
      case 'r':
        go(0,255);
        isValidInput = true;
        break;
      case 's':
        go(0,0);
        isValidInput = true;
        break;
      case '1':
        go(255,-255);
        isValidInput = true;
        break;
      case '2':
        go(-255,255);
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
    analogWrite(LEFT_MOTOR_PIN1,0);
    analogWrite(LEFT_MOTOR_PIN2,0);
    analogWrite(RIGHT_MOTOR_PIN1,0);
    analogWrite(RIGHT_MOTOR_PIN2,0);
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    analogWrite(LEFT_MOTOR_PIN2, 0);
  } 
  else {
    analogWrite(LEFT_MOTOR_PIN1, 0);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    analogWrite(RIGHT_MOTOR_PIN2, 0);
  }else {
    analogWrite(RIGHT_MOTOR_PIN1, 0);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
  }
}
