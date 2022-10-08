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

#include "I2CSubSystem.h"

I2CSubSystem::I2CSubSystem()
{

}

I2CSubSystem::~I2CSubSystem()
{

}

I2CSubSystem::I2CSubSystem(char address)
{
	myAddress = address;
}

/*
	Get the own address of the device.
	return address of this device.
*/
char I2CSubSystem::getMyAddress()
{
	return myAddress;
}

/*
	Set the own address of the device.
*/
void I2CSubSystem::setMyAddress(char address)
{
	myAddress = address;
}

void I2CSubSystem::addLCDDevice(int addressDevice, int columns, int rows)
{
	lcdAddress = addressDevice;
	lcdColumns = columns;
	lcdRows = rows;
}

int I2CSubSystem::getLcdAddress()
{
	return lcdAddress;
}
int I2CSubSystem::getLcdColumns()
{
	return lcdColumns;
}
int I2CSubSystem::getLcdRows()
{
	return lcdRows;
}

void I2CSubSystem::addSystemEEPROMAddress(int addressDevice)
{
	systemEEPROMAddress = addressDevice;
}
int I2CSubSystem::getSystemEEPROMAddress()
{
	return systemEEPROMAddress;
}
void I2CSubSystem::addAccelGyroAddress(int addressDevice)
{
	accelGyroAddress = addressDevice;
}

int I2CSubSystem::getAccelGyroAddress()
{
	return accelGyroAddress;
}

void  I2CSubSystem::addMagneticCompassAddress(int addressDevice)
{
	magneticCompassAddress = addressDevice;
}

int  I2CSubSystem::getMagneticCompassAddress()
{
	return magneticCompassAddress;
}

