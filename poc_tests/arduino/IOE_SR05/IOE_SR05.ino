/*
 * IOE-SR05 using NeoSWSerial.
 * Copyright 2024 Gabriel Dimitriu
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
#include <Arduino.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 12
#define DistanceEn_Pin 3

NeoSWSerial SR05Serial(RxD, TxD);
uint8_t S_DATA = 0;
uint8_t H_DATA = 0;
uint8_t L_DATA = 0;
uint8_t SUM = 0;
unsigned long distance = 0;

void neoSSerial1ISR() {
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

void setup() {
  SR05Serial.begin(9600);
  Serial.begin(9600);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  pinMode(DistanceEn_Pin,OUTPUT);
  digitalWrite(DistanceEn_Pin,1);
}

void getDistance() {
  digitalWrite(DistanceEn_Pin,0);
  while( SR05Serial.available() == 0 );
  S_DATA = SR05Serial.read();
  if ( S_DATA == 0xFF )  {
    H_DATA = SR05Serial.read();
    L_DATA = SR05Serial.read();
    SUM = SR05Serial.read();
  } else {
    Serial.print("dumping data: ");
    Serial.print(S_DATA, HEX);
    Serial.print(" ");
    while( SR05Serial.available() > 0 ) {
      Serial.print(SR05Serial.read(), HEX);
      Serial.print(" ");
    }
    Serial.println();
    Serial.flush();
  }
  digitalWrite(DistanceEn_Pin,1);
  distance = H_DATA * 256  +  L_DATA;
  uint16_t sum = S_DATA + H_DATA + L_DATA;
  sum = sum << 8;
  sum = sum >> 8; 
  if ( SUM == sum ) {
    Serial.print("The distance is : ");
    Serial.print(distance);
    Serial.println(" mm");
  }
  Serial.print(S_DATA, HEX);
  Serial.print(" ");
  Serial.print(H_DATA, HEX);
  Serial.print(" ");
  Serial.print(L_DATA, HEX);
  Serial.print(" ");
  Serial.print(SUM, HEX);
  Serial.print(" ");
  Serial.println(distance);
}

void loop() {
 while(Serial.available() == 0 );
 Serial.read();
 Serial.println("Reading distance");
 getDistance();
}
