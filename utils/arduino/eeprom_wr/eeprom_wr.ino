#include <Wire.h>
unsigned long Len_mm  = 0;
unsigned int address  = 0;

byte readByte_EEPROM(int deviceAddress, unsigned int address)
{
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceAddress);
  Wire.write((int) (address >>8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  return rdata;
}

void writeByte_EEPROM(int deviceAddress, unsigned int address, byte data)
{
  int rdata  = data;
  Wire.beginTransmission(deviceAddress);
  Wire.write((int) (address >> 8)); //MSB
  Wire.write((int) (address & 0xFF)); //LSB
  Wire.write(rdata);
  Wire.endTransmission();
}

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
   Serial.begin(9600);   

}

void loop() {
  // put your main code here, to run repeatedly:

}
