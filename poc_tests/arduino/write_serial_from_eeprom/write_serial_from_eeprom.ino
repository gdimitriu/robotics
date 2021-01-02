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

byte citesteByte_EEPROM(int adresaDevice, unsigned int adresa)
{
  byte rdata = 0xFF;
  Wire.beginTransmission(adresaDevice);
  Wire.write((int) (adresa >>8)); //MSB
  Wire.write((int) (adresa & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(adresaDevice, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  return rdata;
}

void citesteBuffer_EEPROM(int adresaDevice, unsigned int adresa, byte * buffer, int lungime)
{
  Wire.beginTransmission(adresaDevice);
  Wire.write((int)(adresa >> 8)); //MSB
  Wire.write((int) (adresa & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(adresaDevice, lungime);
  int idx = 0;
  for(idx = 0; idx < lungime; idx++) {
    if (Wire.available()) {
      buffer[idx] = Wire.read();
    }
  }
}

void scriePagina_EEPROM(int adresaDevice, unsigned int adresaPagina, byte *data, byte lungime)
{
  Wire.beginTransmission(adresaDevice);
  Wire.write((int) (adresaPagina >> 8)); //MSB
  Wire.write((int) (adresaPagina & 0xFF)); //LSB
  byte idx;
  for(idx = 0; idx < lungime; idx++) {
    Wire.write(data[idx]);
  }
  Wire.endTransmission();
}

void scrieByte_EEPROM(int adresaDevice, unsigned int adresa, byte data)
{
  int rdata  = data;
  Wire.beginTransmission(adresaDevice);
  Wire.write((int) (adresa >> 8)); //MSB
  Wire.write((int) (adresa & 0xFF)); //LSB
  Wire.write(rdata);
  Wire.endTransmission();
}
void setup() {
  char vectDate[] = "Hi from Arduino!";
  Wire.begin();
  Serial.begin(9600);
  scriePagina_EEPROM(0x50, 0, (byte *)vectDate, sizeof(vectDate));
  delay(100);
  Serial.println("Memoria a fost scrisa!");

}

void loop() {
  int addr = 0; //first address
  byte octet = citesteByte_EEPROM(0x50, 0);
  while(octet != 0) {
    Serial.print((char)octet);
    addr++; //increase adress
    octet = citesteByte_EEPROM(0x50, addr);
  }
  Serial.println(" ");
  delay(2000);
}
