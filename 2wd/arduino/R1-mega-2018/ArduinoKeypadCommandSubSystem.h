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
#ifndef ArduinoKeypadCommandSubSystem_h
#define ArduinoKeypadCommandSubSystem_h
#include "CommandSubSystem.h"
#include "ArduinoKeypadSettings.h"
#include <Keypad.h>

class ArduinoKeypadCommandSubSystem : public CommandSubSystem
{
public:
	ArduinoKeypadCommandSubSystem(I2CSubSystem *i2cSystem, ArduinoKeypadSettings *settings);
	static void enableCommandISR();
	virtual bool isEnabledCommandISR();
	virtual bool isReady();
	virtual char * getCommand();
	virtual char * getValue();
	virtual void reset(char *command);
	static bool interruptEnabled;
private:
	bool ready = false;
	char * command;
	char * value;
	int indexCommand;
	int indexValue;
	bool hasCommand;
	bool isFloatValueCommand = false;
	string *messages;
	Keypad keypad;
	ArduinoKeypadSettings * keyPadSettings;
};
#endif
