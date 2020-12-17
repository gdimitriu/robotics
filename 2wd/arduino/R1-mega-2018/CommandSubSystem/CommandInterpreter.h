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
#ifndef CommandInterpreter_h
#define CommandInterpreter_h

#include <AutonomousDroidSettings.h>
#include <NavigationSubSystem.h>
#include "CommandExecutionList.h"

class CommandInterpreter
{
public:
	CommandInterpreter();
	virtual bool isCommandAvailable(char *command);
	virtual bool isFloatValueCommand(char *command);
	virtual bool setCommandValue(char *command, char * value);
	virtual string *getCommandValue(char * command);
	void setDroidSettings(AutonomousDroidSettings *settings);
	void setNavigationSubSystem(NavigationSubSystem *navigation);

protected:

private:
	AutonomousDroidSettings * droidSettings = nullptr;
	NavigationSubSystem * navigationSubSystem = nullptr;
	//list of commands and setter/getter
	CommandExecutionList < AutonomousDroidSettings, void (AutonomousDroidSettings::*)(unsigned long), unsigned long (AutonomousDroidSettings::*)(void)> *droidSettingsCommandList = nullptr;
	CommandExecutionList < NavigationSubSystem, void (NavigationSubSystem::*)(float), float (NavigationSubSystem::*)(void)> *navigationCommandList = nullptr;
};
#endif