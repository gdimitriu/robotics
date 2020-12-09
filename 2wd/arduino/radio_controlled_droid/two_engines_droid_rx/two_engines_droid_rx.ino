/*
Radio Controlled Droid
Copyright 2020 Gabriel Dimitriu

This file is part of Radio Controlled Droid.
Two engine controlled by radio using joystick the receiver part.
Thi use L298N micro driver for engines (the one with two wires).

Radio Controlled Droid is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Radio Controlled Droid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Radio Controlled Droid; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <VirtualWire.h>
#define LEFT_MOTOR_PIN1 5
#define LEFT_MOTOR_PIN2 6
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11
#define PIN_RX 8
#define BUFFER 40
#define MINIMUM_POWER 170
char inData[BUFFER];
char xData[BUFFER];
char yData[BUFFER];

#define DEBUG_MODE true
//#define DISABLE_ENGINES true
void setup() {
#ifdef DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");
#endif
  vw_set_ptt_inverted(true);
  vw_set_rx_pin(PIN_RX); //Set transmitter pin
  vw_setup(400); //Set transmission speed, slower means greater range
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  go(0,0);
  vw_rx_start();
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
#ifndef DISABLE_ENGINES
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef DEBUG_MODE
    Serial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef DEBUG_MODE
    Serial.print("Left ");Serial.print(speedLeft);Serial.print(" ");Serial.println(0);
#endif    
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef DEBUG_MODE
    Serial.print("Left ");Serial.print(0); Serial.print(" ");Serial.println(-speedLeft);
#endif    
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef DEBUG_MODE
    Serial.print("Right "); Serial.print(speedRight); Serial.print(" "); Serial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef DEBUG_MODE
    Serial.print("Right "); Serial.print(0); Serial.print(" "); Serial.println(-speedRight);
#endif    
  }
#endif
}

void moveX() {
  int power;
  for (unsigned int i = 0 ; i < strlen(inData); i++) {
     inData[i]=inData[i+1];
  }
  power = atoi(inData);
#ifdef DEBUG_MODE    
  Serial.print("Move linear with power ");Serial.println(power);
#endif
  go(power,power);
}

void moveY() {
  int power;
  for (unsigned int i = 0 ; i < strlen(inData); i++) {
     inData[i]=inData[i+1];
  }
  power = atoi(inData);
#ifdef DEBUG_MODE
  Serial.print("Turn on spot with power ");Serial.println(power);
#endif
  if (power > 0) { //right
     go(power,-power);
  } else {
     go(power, -power);
  }
}

int normalizeLower(int power) {
  if (power < -MINIMUM_POWER) {
      return  -MINIMUM_POWER;
  } else if (power > 0 && power < MINIMUM_POWER) {
      return  MINIMUM_POWER;
  } else  {
    return power;
  }
}
void moveWithTurn() {
  for (unsigned int i = 0 ; i < strlen(inData); i++) {
     inData[i]=inData[i+1];
  }
  int xLength = 0;
  int yLength = 0;
  boolean isY = false;
  for (unsigned int i = 0 ; i < strlen(inData); i++) {
     if (inData[i]==',') {
        xData[xLength]='\0';
        isY = true;
     }
     if (!isY) {
        xData[xLength] = inData[i];
        xLength++;
     } else if (inData[i] != ',') {
        yData[yLength] = inData[i];
        yLength++;
     }
  }
  yData[yLength]='\0';
  int powerX = atoi(xData);
  int powerY = atoi(yData);
  if (powerY > 0) { 
    //turn right
    if (powerX > 0 ) {
      //move forward
#ifdef DEBUG_MODE
  Serial.print("Move forward right : left=");Serial.print(powerX);Serial.print(" right=");Serial.println(normalizeLower(labs(powerX-powerY/2)));
#endif      
     go(powerX,normalizeLower(labs(powerX-powerY/2)));
    } else {
      
    }
  } else {
    //turn left
    if (powerX > 0) {
      //move forward
#ifdef DEBUG_MODE
  Serial.print("Move forward left : left=");Serial.print(normalizeLower(labs(powerX+powerY/2)));Serial.print(" right=");Serial.println(powerX);
#endif      
     go(normalizeLower(labs(powerX+powerY/2)),powerX);
    } else {
      
    }
  }
}

void loop() {
  uint8_t bufLen = BUFFER;
  for(int i = 0; i < BUFFER; i++) {
    inData[i]='\0';
    xData[i]='\0';
    yData[i]='\0';
  }
  if(!vw_get_message(inData,&bufLen)) {
    return;
  }
#ifdef DEBUG_MODE
  Serial.println(inData);
#endif
  if (strcmp(inData,"s") == 0) {
#ifdef DEBUG_MODE    
    Serial.println("stop");
#endif
    go(0,0);
  } else if (strlen(inData) > 1) {
    if (inData[0] == 'x') {
      moveX();
    } else if (inData[0] == 'y') {
      moveY();
    } else if (inData[0] == 'c') {
      moveWithTurn();
    }
  }
}
