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

#include "OnionI2CSubSystem.h"
#include <cstdlib.h>

OnionI2CSubSystem::OnionI2CSubSystem()
{

}

OnionI2CSubSystem::~OnionI2CSubSystem()
{

}

OnionI2CSubSystem::OnionI2CSubSystem(char address) : I2CSubSystem(address)
{

}

/*
	I2C EEPROM operations.
*/
int OnionI2CSubSystem::readByte_EEPROM(int addressDevice, unsigned int address)
{
	int rdata = 0xFF;
	i2c_readByte(0, addressDevice, address, &rdata)
	return rdata;
}

int OnionI2CSubSystem::readBuffer_EEPROM(int addressDevice, unsigned int address, int * buffer, int lengthBuff)
{
	int status = i2c_read(0, addressDevice, address, buffer, lengthBuff);
	if (status == EXIT_SUCCESS) {
		return lengthBuff;
	}
	else {
		return 0;
	}
}

void OnionI2CSubSystem::writePage_EEPROM(int addressDevice, unsigned int addressPage, int *data, int lengthBuff)
{
	int status = i2c_writeBuffer(0, addressDevice, addressPage, buffer, lengthBuff);
	if (status == EXIT_SUCCESS) {
		return lengthBuff;
	}
	else {
		return 0;
	}
}

void OnionI2CSubSystem::writeByte_EEPROM(int addressDevice, unsigned int address, int data)
{
	int status = i2c_writeBytes(0, addressDevice, address, data, 1);
	if (status == EXIT_SUCCESS) {
		return 1;
	}
	else {
		return 0;
	}
}