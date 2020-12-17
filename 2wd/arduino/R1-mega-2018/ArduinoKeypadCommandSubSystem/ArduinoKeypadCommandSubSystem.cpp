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

#include <ArduinoKeypadCommandSubSystem.h>

bool ArduinoKeypadCommandSubSystem::interruptEnabled = false;

void ArduinoKeypadCommandSubSystem::enableCommandISR()
{
	interruptEnabled = true;
}

ArduinoKeypadCommandSubSystem::ArduinoKeypadCommandSubSystem(I2CSubSystem *i2cSystem, ArduinoKeypadSettings *settings) : CommandSubSystem(i2cSystem)
{
	this->keyPadSettings = settings;
	setI2CSubSystem(i2cSystem);
	keypad =Keypad(settings->getKeyMap(), settings->getRowPins(), settings->getColPins(), settings->getNrRows(), settings->getNrColumns());
	pinMode(keyPadSettings->getCommandButton(), INPUT);
	attachInterrupt(digitalPinToInterrupt(keyPadSettings->getCommandButton()), ArduinoKeypadCommandSubSystem::enableCommandISR, RISING);
	interruptEnabled = false;
	ready = false;
	indexCommand = 0;
	indexValue = 0;
	hasCommand = false;
	command = calloc(17, sizeof(char));
	value = calloc(17, sizeof(char));
	interrupts();
}

bool ArduinoKeypadCommandSubSystem::isEnabledCommandISR()
{
	return interruptEnabled;
}

bool ArduinoKeypadCommandSubSystem::isReady()
{
	if (!interruptEnabled) {
		return false;
	}
	if (ready) {
		return true;
	}
	char key = NO_KEY;
	if (!hasCommand) {
		key = keypad.waitForKey();
	}
	else {
		key = keypad.getKey();
	}
	if (key == NO_KEY) return false;
	
	if (indexCommand == 0 && key != '*' && hasCommand == false) {
		return false;
	}
	if (indexCommand == 0 && key == '*') {
		hasCommand = true;
		return false;
	}
	if (hasCommand) {
		if (key == '*') {
			command[indexCommand] = '\0';
			hasCommand = false;
			if (!commandInterpreter->isCommandAvailable(command)) {
				getI2CSubSystem()->printRow("unavailable command", 0);
				indexCommand = 0;
				hasCommand = false;
				delay(300);
				getI2CSubSystem()->clearLCD();
				return false;
			}
			isFloatValueCommand = commandInterpreter->isFloatValueCommand(command);
			string * tmp = commandInterpreter->getCommandValue(command);
			getI2CSubSystem()->printRow(tmp->c_str(), 1);
			delete tmp;
			return false;
		}
		command[indexCommand] = key;
		indexCommand++;
		getI2CSubSystem()->printRow(command, 0);
	}
	else {
		if (key == '#') {
			value[indexValue] = '\0';
			ready = true;
			commandInterpreter->setCommandValue(command, value);
			return true;
		}
		if (isFloatValueCommand && key =='*') {
			value[indexValue] = '.';
		}
		else {
			value[indexValue] = key;
		}
		indexValue++;
		getI2CSubSystem()->printRow(value, 2);
	}
	return false;
}

char * ArduinoKeypadCommandSubSystem::getCommand()
{
	return command;
}

char * ArduinoKeypadCommandSubSystem::getValue()
{
	return value;
}

void ArduinoKeypadCommandSubSystem::reset(char *command)
{
	delay(1000);
	getI2CSubSystem()->clearLCD();
	if (!commandInterpreter->isCommandAvailable(command)) {
		return;
	}
	indexCommand = 0;
	indexValue = 0;
	ready = false;
	interruptEnabled = false;
	hasCommand = false;
	for (int i = 0; i < 17; i++) {
		command[i] = '\0';
		value[i] = '\0';
	}
}