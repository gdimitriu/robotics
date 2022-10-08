/*
 * Pixy2 with arduino uno to move from Left barcode to Right barcode.
 * @2019 Gabriel Dimitriu
 * 
*/
//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#include <Pixy2.h>

int LEFT_MOTOR_PIN1 = 6;
int LEFT_MOTOR_PIN2 = 5;
int LEFT_MOTOR_SPEED = 4;
int RIGHT_MOTOR_PIN1 = 7;
int RIGHT_MOTOR_PIN2 = 8;
int RIGHT_MOTOR_SPEED = 9;

Pixy2 pixy;
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
void setup() 
{
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  Serial.begin(115200);
  Serial.print("Starting...\n");
  go(0,0);
  pixy.init();
  // Turn on both lamps, upper and lower for maximum exposure
  pixy.setLamp(0, 0);
  // change to the line_tracking program.  Note, changeProg can use partial strings, so for example,
  // you can change to the line_tracking program by calling changeProg("line") instead of the whole
  // string changeProg("line_tracking")
  pixy.changeProg("line");
}


void loop()
{
  int8_t res;
  int32_t error; 
  int left, right;
  char buf[96];

  // Get latest data from Pixy, including main vector, new intersections and new barcodes.
  res = pixy.line.getMainFeatures();

  // If error or nothing detected, stop motors
  if (res<=0) 
  {
    go(0,0);
    return;
  }

  if (res&LINE_BARCODE)
  {
    pixy.line.barcodes->print();
    // code==0 is our left-turn sign
    if (pixy.line.barcodes->m_code==0)
    {
      Serial.println("left");
      go(-255,255);
      delay(1100);
      go(0,0);
    }
    // code==5 is our right-turn sign
    else if (pixy.line.barcodes->m_code==5)
    {
      Serial.println("right");
      go(255,-255);
      delay(1100);
      go(0,0);
    }
  }
}
