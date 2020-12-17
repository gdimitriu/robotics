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
#pragma once
#ifndef I2CSubSystem_h
#define I2CSubSystem_h
#include <stdint.h>
/*
	Each processor which communicate with I2C devices should derivate this instance.
	*/
class I2CSubSystem
{
public:
	/*
		Constructor which set the address of the owner.
	*/
	I2CSubSystem(char address);
	I2CSubSystem();
	~I2CSubSystem();
	void setMyAddress(char address);
	char getMyAddress();
	/*
		I2C EEPROM operations.
	*/
	virtual int readByte_EEPROM(int addressDevice, unsigned int address) = 0;
	virtual int readBuffer_EEPROM(int addressDevice, unsigned int address, uint8_t * buffer, int lengthBuff) = 0;
	virtual void writePage_EEPROM(int addressDevice, unsigned int addressPage, uint8_t *data, int lengthBuff) = 0;
	virtual void writeByte_EEPROM(int addressDevice, unsigned int address, int data) = 0;
	//System EEPROM
	virtual int readByte_System_EEPROM(unsigned int address) = 0;
	virtual int readBuffer_System_EEPROM(unsigned int address, uint8_t * buffer, int lengthBuff) = 0;
	virtual void writePage_System_EEPROM(unsigned int addressPage, uint8_t *data, int lengthBuff) = 0;
	virtual void writeByte_System_EEPROM(unsigned int address, int data) = 0;
	
	/*
		subsystems for i2c
	*/
	/*
		LCD SubSystem
	*/
	void addLCDDevice(int addressDevice, int columns, int rows);
	virtual bool hasLCD() = 0;
	int getLcdAddress();
	int getLcdColumns();
	int getLcdRows();
	virtual void print(char** messages, int nrOfMessages) = 0;
	virtual void printRow(char* message, int row) = 0;
	virtual void clearLCD() = 0;
	virtual void enableLCD() = 0;
	virtual void disableLCD() = 0;
	/*
		EEPROM SubSystem
	*/
	void addSystemEEPROMAddress(int addressDevice);
	int getSystemEEPROMAddress();
	/*
		3 axws Accelerometer and Gyroscopic SubSystem
	*/
	void addAccelGyroAddress(int addressDevice);
	int getAccelGyroAddress();
	void addMagneticCompassAddress(int addressDevice);
	int getMagneticCompassAddress();
	virtual void getAcceleration(int* x, int* y, int* z) = 0;
	virtual void getRotation(int* x, int* y, int* z) = 0;
	virtual double getHeading() = 0;

	/*
		Initialize the I2C devices.
	*/
	virtual void initialize() = 0;

private:
	char myAddress;
	int lcdAddress = 0x27;
	int lcdColumns = 16;
	int lcdRows = 2;
	int systemEEPROMAddress = 0x50;
	int accelGyroAddress = 0x68;
	int magneticCompassAddress = 0;
};

#endif