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
#include<KeypadCommand.h>
KeypadCommand::KeypadCommand(Keypad * keypad)
{
	this->keypad = keypad;
	this->enableCommand = false;
	this->ready = false;
	this->indexCommand = 0;
	this->indexValue = 0;
	this->hasCommand = false;
	this->command = calloc(17, sizeof(char));
	this->value = calloc(17, sizeof(char));
}

void KeypadCommand::setEnableCommand()
{
	this->enableCommand = true;
	this->ready = false;
	this->indexCommand = 0;
	this->indexValue = 0;
	this->hasCommand = false;
}

bool KeypadCommand::isReady()
{
	if(!enableCommand) {
		return false;
	}
	if (ready) {
		return true;
	}
	char key = keypad->getKey();
	
	if (key == NO_KEY) {
		return false;
	}
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
			return false;
		}
		command[indexCommand] = key;
		indexCommand++;
	}
	else {
		if (key == '#') {
			ready = true;
			return true;
		}
		value[indexValue] = key;
		indexValue++;
	}
}

void KeypadCommand::reset()
{
	this->indexCommand = 0;
	this->indexValue = 0;
	this->ready = false;
	this->enableCommand = false;
	this->hasCommand = false;
	for (int i = 0; i < 17; i++) {
		this->command[i] = '\0';
		this->value[i] = '\0';
	}
}

char * KeypadCommand::getCommand()
{
	return this->command;
}
char * KeypadCommand::getValue()
{
	return this->value;
}