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
#ifndef OnionI2CSubSystem_h
#define OnionI2CSubSystem_h
#include <I2CSubSystem.h>
#include <onion-i2c.h>
/*
	Each onion processor which communicate with I2C devices should have this instance.
*/
class OnionI2CSubSystem : I2CSubSystem
{
public:
	/*
		Constructor which set the address of the owner.
		Each Onion processor which communicate with I2C devices should have this instance.
	*/
	OnionI2CSubSystem(char address);
	OnionI2CSubSystem();
	~OnionI2CSubSystem();
	/*
		I2C EEPROM operations.
	*/
	int readByte_EEPROM(int addressDevice, unsigned int address);
	int readBuffer_EEPROM(int addressDevice, unsigned int address, int * buffer, int lengthBuff);
	void writePage_EEPROM(int addressDevice, unsigned int addressPage, int *data, int lengthBuff);
	void writeByte_EEPROM(int addressDevice, unsigned int address, int data);
private:
	char my_address;
};

#endif