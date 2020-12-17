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
#ifndef ArduinoI2CSubSystem_h
#define ArduinoI2CSubSystem_h
#include <I2CSubSystem.h>
#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <LiquidCrystal_I2C.h>
#include <MPU6050.h>
#include <HMC5983.h>

/*
	Each ardunino processor which communicate with I2C devices should have this instance.
	There should be a similar instance for the onion omega brain.
*/
class ArduinoI2CSubSystem : public I2CSubSystem
{
public:
	/*
		Constructor which set the address of the owner.
		Each ardunino processor which communicate with I2C devices should have this instance.
	*/
	ArduinoI2CSubSystem(char address);
	ArduinoI2CSubSystem();
	~ArduinoI2CSubSystem();
	/*
		I2C EEPROM operations.
	*/
	int readByte_EEPROM(int addressDevice, unsigned int address);
	int readBuffer_EEPROM(int addressDevice, unsigned int address, uint8_t * buffer, int lengthBuff);
	void writePage_EEPROM(int addressDevice, unsigned int addressPage, uint8_t *data, int lengthBuff);
	void writeByte_EEPROM(int addressDevice, unsigned int address, int data);
	//System EEPROM
	int readByte_System_EEPROM(unsigned int address);
	int readBuffer_System_EEPROM(unsigned int address, uint8_t * buffer, int lengthBuff);
	void writePage_System_EEPROM(unsigned int addressPage, uint8_t *data, int lengthBuff);
	void writeByte_System_EEPROM(unsigned int address, int data);
	/*
		Initialize the I2C devices.
	*/
	void initialize();
	/*
		LCD operations
	*/
	bool hasLCD();
	void print(char** messages, int nrOfMessages);
	void printRow(char* message, int row);
	void clearLCD();
	virtual void enableLCD();
	virtual void disableLCD();


	/*
		3 Axes accelerometre and gyroscopic sensor
	*/
	void getAcceleration(int* x, int* y, int* z);
	void getRotation(int* x, int* y, int* z);

	/*
		Magnetic compass
	*/
	double getHeading();
private:
	LiquidCrystal_I2C *lcdDevice = nullptr;
	MPU6050 *accelGyro = nullptr;
	HMC5983 *magneticCompass = nullptr;
	bool lcdEnabled = true;
};

#endif