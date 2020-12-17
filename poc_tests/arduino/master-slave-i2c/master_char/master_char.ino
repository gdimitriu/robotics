/*
  I2C master test send just one char continous.
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
  Wire.begin();
  Serial.println("Master");
}

void loop() {
  Wire.beginTransmission(1);
  Wire.write('a');
  Wire.endTransmission();
}
