/*
Radio Controlled Droid.
Copyright 2020 Gabriel Dimitriu

This is part of Radio Controlled Droid.
Joystick Command emiter.

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
#include <VirtualWire.h> //include library
#include <EnableInterrupt.h>
#define JOYSTICK_X_PIN A3
#define JOYSTICK_Y_PIN A4
#define JOYSTICK_SW_PIN 4
#define JOYSTICK_X_ZERO 510
#define JOYSTICK_Y_ZERO 539
#define PIN_TX 8
#define BUFFER 20
long joystick_x;
long joystick_x_old;
long joystick_y;
long joystick_y_old;
int joystick_sw;
uint8_t sendData[BUFFER];
#define NR_READS 50
#define DEBUG_MODE true
volatile boolean fullStopFlag = false;
void setup() {
#ifdef DEBUG_MODE  
  Serial.begin(9600);
#endif
  pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_X_PIN, INPUT);
  pinMode(JOYSTICK_Y_PIN, INPUT);
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(PIN_TX); //Set transmitter pin
  vw_setup(400); //Set transmission speed, slower means greater range
  enableInterrupt(JOYSTICK_SW_PIN,fullStop,FALLING);
  joystick_x = JOYSTICK_X_ZERO;
  joystick_y = JOYSTICK_Y_ZERO;
  joystick_x_old = JOYSTICK_X_ZERO;
  joystick_y_old = JOYSTICK_Y_ZERO;
}

void fullStop() {
  fullStopFlag = true;
}

void clearBuffer() {
  for(int i = 0; i < BUFFER; i++) {
   sendData[i]='\0';
  }
}

int normalize(int power) {
  if (power > 255) {
      return  255;
  } else if (power < -255) {
      return  -255;
  } else {
    return power;
  }
}

void loop() {
  long readValue = 0;
  int i;
  int xPower;
  int yPower;
  for (i = 0; i< NR_READS; i++)
    readValue += analogRead(JOYSTICK_X_PIN);
  readValue = readValue/NR_READS;
  if (labs(readValue - joystick_x) > 2) {
    joystick_x_old = joystick_x;
    joystick_x = readValue;
  }
  if (labs(joystick_x_old - JOYSTICK_X_ZERO) <= 2 && labs(JOYSTICK_X_ZERO - joystick_x) <=2) {
    joystick_x_old = joystick_x;
  }
  readValue = 0;
  for (i = 0; i< NR_READS; i++)
    readValue += analogRead(JOYSTICK_Y_PIN);
  readValue = readValue/NR_READS;
  if (labs(readValue - joystick_y) > 2) {
    joystick_y_old = joystick_y;
    joystick_y = readValue;
  }
  //send data
  clearBuffer();
  if (labs(joystick_x_old - joystick_x) > 2 && labs(joystick_y_old - joystick_y) > 2) {
    xPower = normalize(JOYSTICK_X_ZERO-joystick_x);
    yPower = normalize(-joystick_y+JOYSTICK_Y_ZERO);
#ifdef DEBUG_MODE
    Serial.print("x="); Serial.print(xPower);Serial.print(":y="); Serial.print(yPower);
    Serial.print(" orig x=");Serial.print(joystick_x);Serial.print(":");Serial.println(joystick_x);
#endif    
    sprintf(sendData,"c%d,%d",xPower,yPower);
  } else if (labs(joystick_x_old - joystick_x) > 2) {
    xPower = normalize(JOYSTICK_X_ZERO-joystick_x);
#ifdef DEBUG_MODE    
    Serial.print("x="); Serial.print(xPower);Serial.print(":");Serial.println(joystick_x);
#endif    
    sprintf(sendData,"x%d",xPower);
  } else if (labs(joystick_y_old - joystick_y) > 2) {
    yPower = normalize(-joystick_y+JOYSTICK_Y_ZERO);
#ifdef DEBUG_MODE
    Serial.print("y="); Serial.print(yPower);Serial.print(":");Serial.println(joystick_y);
#endif 
    sprintf(sendData,"y%d",yPower);
  } else if (fullStopFlag) {
     fullStopFlag=false;
     clearBuffer();
#ifdef DEBUG_MODE    
    Serial.println("Pushed");
#endif
    sprintf(sendData, "s"); 
  } else {
    return;
  }
  vw_send((uint8_t *)sendData,strlen(sendData));
  vw_wait_tx();
  joystick_x_old = joystick_x;
  joystick_y_old = joystick_y;
}
