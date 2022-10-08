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

#include "ArduinoKeypadSettings.h"

ArduinoKeypadSettings::ArduinoKeypadSettings(int rows, int cols)
{
	this->nrRows = rows;
	this->nrColumns = cols;
}

int ArduinoKeypadSettings::getNrRows()
{
	return nrRows;
}

int ArduinoKeypadSettings::getNrColumns()
{
	return nrColumns;
}

void ArduinoKeypadSettings::setKeyMap(char *keys)
{
	this->keyMap = keys;
}

char * ArduinoKeypadSettings::getKeyMap()
{
	return this->keyMap;
}

void ArduinoKeypadSettings::setRowPins(char *pins)
{
	this->rowPins = pins;
}

char * ArduinoKeypadSettings::getRowPins()
{
	return this->rowPins;
}

void ArduinoKeypadSettings::setColPins(char *pins)
{
	this->colPins = pins;
}

char * ArduinoKeypadSettings::getColPins()
{
	return this->colPins;
}

void ArduinoKeypadSettings::setCommandButton(int button)
{
	commandButton = button;
}
int ArduinoKeypadSettings::getCommandButton()
{
	return commandButton;
}
