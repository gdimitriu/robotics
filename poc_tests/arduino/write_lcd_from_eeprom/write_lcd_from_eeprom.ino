/*
 * Write on LCD the data read from EEPROM.
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
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EEPROM_ADDRESS 0x50
#define LCD_ADDRESS 0x27

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

byte readByte_EEPROM(int addressDevice, unsigned int address)
{
  byte rdata = 0xFF;
  Wire.beginTransmission(addressDevice);
  Wire.write((int) (address >>8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(addressDevice, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  return rdata;
}

void readBuffer_EEPROM(int addressDevice, unsigned int address, byte * buffer, int lengthBuff)
{
  Wire.beginTransmission(addressDevice);
  Wire.write((int)(address >> 8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(addressDevice, lengthBuff);
  int idx = 0;
  for(idx = 0; idx < lengthBuff; idx++) {
    if (Wire.available()) {
      buffer[idx] = Wire.read();
    }
  }
}

void writePage_EEPROM(int addressDevice, unsigned int addressPage, byte *data, byte lengthBuff)
{
  Wire.beginTransmission(addressDevice);
  Wire.write((int) (addressPage >> 8)); //MSB
  Wire.write((int) (addressPage & 0xFF)); //LSB
  byte idx;
  for(idx = 0; idx < lengthBuff; idx++) {
    Wire.write(data[idx]);
  }
  Wire.endTransmission();
}

void writeByte_EEPROM(int addressDevice, unsigned int address, byte data)
{
  int rdata  = data;
  Wire.beginTransmission(addressDevice);
  Wire.write((int) (address >> 8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.write(rdata);
  Wire.endTransmission();
}

void setup() {
  char vectDate[] = "Hi from Arduino!";
  Wire.begin();
  Serial.begin(9600);
  lcd.begin();
  lcd.setCursor(0,0);
  writePage_EEPROM(EEPROM_ADDRESS, 0, (byte *)vectDate, sizeof(vectDate));
  delay(100);
  Serial.println("Memoria a fost scrisa!");

}

void loop() {
  int addr = 0; //first address
  byte octet = readByte_EEPROM(EEPROM_ADDRESS, 0);
  int row = 0;
  int column = 0;
  lcd.setCursor(0,0);
  while(octet != 0) {
    Serial.print((char)octet);
    lcd.write((char) octet);
    addr++; //increase adress
    column++;
    octet = readByte_EEPROM(EEPROM_ADDRESS, addr);
    if (column == 16) {
      column = 0;
      if (row == 1) {
        row = 0;
        delay(5000);
      } else {
        row = 1;
      }
    }
    lcd.setCursor(column, row);
  }
  Serial.println(" ");
  delay(10000);
}
