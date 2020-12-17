/*
 * Copyright 2018 Gabriel Dimitriu

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
 */
#include <Keypad.h>
#include <PinChangeInt.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte colPins[ROWS] = {6,5,4,3}; //connect to the row pinouts of the keypad
byte rowPins[COLS] = {11, 10, 9,8 }; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys),rowPins, colPins, ROWS, COLS );

void setup(){
  Serial.begin(9600);
  interrupts ();
  attachInterrupt(digitalPinToInterrupt(2), myKeyPad, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), myKeyPad, CHANGE);
}
  
void loop(){
}

void myKeyPad() {
  char key = keypad.getKey();
  
  if (key){
    Serial.println(key);
  }
}
