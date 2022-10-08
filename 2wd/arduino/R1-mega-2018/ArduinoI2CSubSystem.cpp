/*
 * Arduino I2CSubSystem.
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

#include "ArduinoI2CSubSystem.h"

ArduinoI2CSubSystem::ArduinoI2CSubSystem()
{

}

ArduinoI2CSubSystem::~ArduinoI2CSubSystem()
{
	if (lcdDevice != NULL) delete lcdDevice;
}

ArduinoI2CSubSystem::ArduinoI2CSubSystem(char address) : I2CSubSystem(address)
{

}

void ArduinoI2CSubSystem::initialize()
{
	Wire.begin(getMyAddress());
	if (getLcdAddress() != 0 && getLcdColumns() != 0 && getLcdRows() != 0) {
		lcdDevice = new LiquidCrystal_I2C(getLcdAddress(), getLcdColumns(), getLcdRows());
		lcdDevice->begin();
		disableLCD();
	}
	if (getAccelGyroAddress() != 0) {
		accelGyro = new MPU6050(getAccelGyroAddress());
		accelGyro->initialize();
	}
	if (getMagneticCompassAddress() != 0) {
		magneticCompass = new HMC5983();
		magneticCompass->begin();
	}
}

/*
	I2C EEPROM operations.
*/
int ArduinoI2CSubSystem::readByte_EEPROM(int addressDevice, unsigned int address)
{
	int rdata = 0xFF;
	Wire.beginTransmission(addressDevice);
	Wire.write((int)(address >> 8)); //MSB
	Wire.write((int)(address & 0xFF)); //LSB
	Wire.endTransmission();
	Wire.requestFrom(addressDevice, 1);
	if (Wire.available()) {
		rdata = Wire.read();
	}
	return rdata;
}

int ArduinoI2CSubSystem::readBuffer_EEPROM(int addressDevice, unsigned int address, uint8_t * buffer, int lengthBuff)
{
	Wire.beginTransmission(addressDevice);
	Wire.write((int)(address >> 8)); //MSB
	Wire.write((int)(address & 0xFF)); //LSB
	Wire.endTransmission();
	Wire.requestFrom(addressDevice, lengthBuff);
	int idx = 0;
	int readBytes = 0;
	for (idx = 0, readBytes = 0; idx < lengthBuff; idx++) {
		if (Wire.available()) {
			buffer[idx] = Wire.read();
			readBytes++;
		}
	}
	return idx;
}

void ArduinoI2CSubSystem::writePage_EEPROM(int addressDevice, unsigned int addressPage, uint8_t *data, int lengthBuff)
{
	Wire.beginTransmission(addressDevice);
	Wire.write((int)(addressPage >> 8)); //MSB
	Wire.write((int)(addressPage & 0xFF)); //LSB
	int idx;
	for (idx = 0; idx < lengthBuff; idx++) {
		Wire.write(data[idx]);
	}
	Wire.endTransmission();
}

void ArduinoI2CSubSystem::writeByte_EEPROM(int addressDevice, unsigned int address, int data)
{
	Wire.beginTransmission(addressDevice);
	Wire.write((int)(address >> 8)); //MSB
	Wire.write((int)(address & 0xFF)); //LSB
	Wire.write(data);
	Wire.endTransmission();
}
/*
	Operation on the system EEPROM.
*/
int ArduinoI2CSubSystem::readByte_System_EEPROM(unsigned int address)
{
	return readByte_EEPROM(getSystemEEPROMAddress(), address);
}
int ArduinoI2CSubSystem::readBuffer_System_EEPROM(unsigned int address, uint8_t * buffer, int lengthBuff)
{
	return readBuffer_EEPROM(getSystemEEPROMAddress(), address, buffer, lengthBuff);
}
void ArduinoI2CSubSystem::writePage_System_EEPROM(unsigned int addressPage, uint8_t *data, int lengthBuff)
{
	writePage_EEPROM(getSystemEEPROMAddress(), addressPage, data, lengthBuff);
}
void ArduinoI2CSubSystem::writeByte_System_EEPROM(unsigned int address, int data)
{
	writeByte_EEPROM(getSystemEEPROMAddress(), address, data);
}

/*
	LCD SubSystem
*/
bool ArduinoI2CSubSystem::hasLCD()
{
	if (lcdDevice != nullptr) return true;
	return false;
}

void ArduinoI2CSubSystem::print(char** messages, int nrOfMessages)
{
	if (lcdDevice == nullptr) return;
	lcdDevice->clear();
	for (int i = 0; i < nrOfMessages; i++) {
		lcdDevice->setCursor(0, i);
		lcdDevice->print(messages[i]);
	}
}
void ArduinoI2CSubSystem::printRow(char* message, int row)
{
	if (lcdDevice == nullptr) return;
	lcdDevice->setCursor(0, row);
	lcdDevice->print(message);
}

void ArduinoI2CSubSystem::clearLCD()
{
	if (lcdDevice == nullptr) return;
	lcdDevice->clear();
}

void ArduinoI2CSubSystem::enableLCD()
{
	if (lcdDevice == nullptr || lcdEnabled == true) return;
	lcdEnabled = true;
	lcdDevice->display();
	lcdDevice->backlight();
	lcdDevice->clear();
}

void ArduinoI2CSubSystem::disableLCD()
{
	if (lcdDevice == nullptr || lcdEnabled == false) return;
	lcdEnabled = false;
	lcdDevice->noDisplay();
	lcdDevice->noBacklight();
}

/*
	3 motion SubSystem
*/
void ArduinoI2CSubSystem::getAcceleration(int *x, int *y, int *z)
{
	if (accelGyro != nullptr) {
		accelGyro->getAcceleration(x, y, z);
	}
}
void ArduinoI2CSubSystem::getRotation(int *x, int *y, int *z)
{
	if (accelGyro != nullptr) {
		accelGyro->getRotation(x, y, z);
	}
}

/*
	Magnetic compass
*/
double ArduinoI2CSubSystem::getHeading()
{
	if (magneticCompass != nullptr) {
		return magneticCompass->read();
	}
	return 0.0f;
}