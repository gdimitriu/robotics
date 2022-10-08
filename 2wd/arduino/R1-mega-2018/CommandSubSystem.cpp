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

#include "CommandSubSystem.h"

CommandSubSystem::CommandSubSystem(I2CSubSystem *i2cSystem)
{
	this->i2cSubSystem = i2cSubSystem;
}

void CommandSubSystem::setI2CSubSystem(I2CSubSystem *i2cSystem)
{
	i2cSubSystem = i2cSystem;
}

I2CSubSystem * CommandSubSystem::getI2CSubSystem()
{
	return i2cSubSystem;
}

void CommandSubSystem::setAutonomousDroidSettings(AutonomousDroidSettings * settings)
{
	this->droidSettings = settings;
	if (this->commandInterpreter != nullptr) {
		this->commandInterpreter->setDroidSettings(this->droidSettings);
	}
}

void CommandSubSystem::setNavigationSubSystem(NavigationSubSystem * settings)
{
	this->navigationSubSystem = settings;
	if (this->commandInterpreter != nullptr) {
		this->commandInterpreter->setNavigationSubSystem(this->navigationSubSystem);
	}
}

void CommandSubSystem::setCommandInterpreter(CommandInterpreter * interpreter)
{
	this->commandInterpreter = interpreter;
}
