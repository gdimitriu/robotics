/*
  I2C slave.
  Copyright 2018 Gabriel Dimitriu

  This is part of Robotics
  
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
#include <Wire.h>
#define SLAVE_ADDRESS 0x01
char * buffer;
void setup() {
  Wire.begin(SLAVE_ADDRESS);                // join i2c bus with address #3
  buffer = calloc(17,sizeof(char));
  Serial.begin(9600);           // start serial for output
}

void loop() {
  while(Serial.available() > 0) {
    String str = Serial.readString();
    str.toCharArray(buffer,17);
    Serial.println(str);
    Wire.beginTransmission(0);
    Wire.write(buffer, 17);
    Wire.endTransmission();
  }
}
