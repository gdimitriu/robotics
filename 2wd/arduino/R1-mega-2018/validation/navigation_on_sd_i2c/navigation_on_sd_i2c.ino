/*
  Put the navigation data to the SD.
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
#include <I2CSubSystem.h>
#include <ArduinoI2CSubSystem.h>
#include <SD.h> 

#define ACCELGYRO_ADDRESS 0x68
#define MAGNETIC_COMPASS_ADDRESS 0x1E

I2CSubSystem *i2cBus;

int16_t ax, ay, az;
int16_t gx, gy, gz;
File accelFile;
File gyroFile;
File compassFile;
double heading;
char *message;

void setup() {
  Serial.begin(9600);
  i2cBus = new ArduinoI2CSubSystem(0x01);
  i2cBus->addAccelGyroAddress(ACCELGYRO_ADDRESS);
  i2cBus->addMagneticCompassAddress(MAGNETIC_COMPASS_ADDRESS);
  i2cBus->initialize();
  SD.begin(4);
  message = calloc(18, sizeof(char));
}
void loop() {
  // put your main code here, to run repeatedly:
  i2cBus->getAcceleration(&ax,&ay,&az);
  i2cBus->getRotation(&gx,&gy,&gz);
  heading = i2cBus->getHeading();
  String lcd = String(ax) + ":" + String(ay) +":" + String(az);
  accelFile = SD.open("/accel.dat", FILE_WRITE);
  accelFile.println(lcd);
  Serial.println(lcd);
  gyroFile = SD.open("/gyro.dat", FILE_WRITE);
  lcd = String(gx) + ":" + String(gy) + ":" + String(gz);
  gyroFile.println(lcd);
  Serial.println(lcd);
  compassFile = SD.open("/compass.dat", FILE_WRITE);
  compassFile.println(heading);
  String(heading).toCharArray(message, 18);
  i2cBus->printRow(message, 0);
  Serial.println(heading);
  accelFile.close();
  gyroFile.close();
  compassFile.close();
  delay(100);
}
