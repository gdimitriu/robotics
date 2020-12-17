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
#ifndef CommandSubSystem_h
#define CommandSubSystem_h

#include <I2CSubSystem.h>
#include <CommandInterpreter.h>
#include <AutonomousDroidSettings.h>
#include <NavigationSubSystem.h>


class CommandSubSystem
{
public:
	CommandSubSystem(I2CSubSystem *i2cSystem);
	void setI2CSubSystem(I2CSubSystem *i2cSystem);
	I2CSubSystem * getI2CSubSystem();
	void setAutonomousDroidSettings(AutonomousDroidSettings * settings);
	void setNavigationSubSystem(NavigationSubSystem * settings);
	void setCommandInterpreter(CommandInterpreter * interpreter);
	virtual bool isEnabledCommandISR() = 0;
	virtual bool isReady() = 0;
	virtual char * getCommand() = 0;
	virtual char * getValue() = 0;
	virtual void reset(char *command)= 0;
private:
	I2CSubSystem *i2cSubSystem = nullptr;
protected:
	AutonomousDroidSettings * droidSettings = nullptr;
	NavigationSubSystem * navigationSubSystem = nullptr;
	CommandInterpreter * commandInterpreter = nullptr;
};

#endif