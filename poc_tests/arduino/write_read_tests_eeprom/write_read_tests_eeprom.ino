/*
 * Write on serial from EEPROM.
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
#include <Wire.h>


void readBytes_EEPROM(int deviceAddress, unsigned int address, byte * buffer, int length)
{  
  Wire.beginTransmission(deviceAddress);
  Wire.write((int)(address >> 8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, length);
  int idx = 0;
  for(idx = 0; idx < length; idx++) {
    if (Wire.available()) {
      buffer[idx] = Wire.read();
    }
  }
}

void writeBytes_EEPROM(int deviceAddress, unsigned int address, byte *data, int length)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write((int) (address >> 8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  byte idx;
  for(idx = 0; idx < length; idx++) {
    Wire.write(data[idx]);
  }
  Wire.endTransmission();
}

void setup() {
  uint16_t val2;
  Wire.setClock(400000);
  Wire.begin();
  Serial.begin(9600);
  val2 = 20367;
//  writeBytes_EEPROM(0x50, 40, (byte *)&val2,2);
  uint16_t val1  = 13670;
  writeBytes_EEPROM(0x50, 60, (byte *)&val1, 2);
  delay(100);
  Serial.println("EEPROM has been written!");
  Serial.println("Read from memory");
  readBytes_EEPROM(0x50,60,(byte *)&val1,2);
  Serial.println(val1);
  delay(100);
  readBytes_EEPROM(0x50,40,(byte *)&val2,2);
  Serial.println(val2);
}

void loop() {
}
