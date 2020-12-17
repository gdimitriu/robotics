/*
  I2C slave test send just receive char continous.
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
#include<Wire.h>
void setup() {
  Serial.begin(9600);
  Wire.begin(1);
  Serial.println("Slave");
  Wire.onReceive(receiveHandler);
}

void loop() {
  
}

void receiveHandler(int nrOfBytes) {
  while(Wire.available()) {
    char c = Wire.read();
    Serial.println(c);
  }
}
