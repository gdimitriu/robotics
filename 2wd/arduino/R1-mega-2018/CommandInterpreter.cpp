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

#include "CommandInterpreter.h"

CommandInterpreter::CommandInterpreter()
{

}

void CommandInterpreter::setDroidSettings(AutonomousDroidSettings *settings)
{
	this->droidSettings = settings;
	droidSettingsCommandList = droidSettings->getCommandList();
}

void CommandInterpreter::setNavigationSubSystem(NavigationSubSystem *navigation)
{
	this->navigationSubSystem = navigation;
	navigationCommandList = navigationSubSystem->getCommandList();
}

bool CommandInterpreter::isCommandAvailable(char *command)
{
	string *temp = new string(command);
	bool rez = droidSettingsCommandList->isAvailable(temp);
	if (!rez) {
		rez = navigationCommandList->isAvailable(temp);
	}
	delete temp;
	return rez;
}

bool CommandInterpreter::isFloatValueCommand(char *command)
{
	string *temp = new string(command);
	if (droidSettingsCommandList->getArgumentType() == 'f') {
		delete temp;
		return true;
	}
	if (navigationCommandList->getArgumentType() == 'f') {
		delete temp;
		return true;
	}
	return false;
}

bool  CommandInterpreter::setCommandValue(char *command, char * value)
{
	string *temp = new string(command);
	bool rez= false;
	int intValue = 0;
	float floatValue = 0.0f;
	if (droidSettingsCommandList->isAvailable(temp)) {
		switch (droidSettingsCommandList->getArgumentType()) {
		case 'l':
			intValue = atol(value);
			rez = droidSettingsCommandList->setValue(temp, intValue);
			break;
		case 'f':
			floatValue = atof(value);
			rez = droidSettingsCommandList->setValue(temp, intValue);
			break;
		}
	}
	else if (navigationCommandList->isAvailable(temp)) {
		switch (navigationCommandList->getArgumentType()) {
		case 'l':
			intValue = atol(value);
			rez = navigationCommandList->setValue(temp, intValue);
			break;
		case 'f':
			floatValue = atof(value);
			rez = navigationCommandList->setValue(temp, intValue);
			break;
		}
	}
	delete temp;
	return rez;
}

string *CommandInterpreter::getCommandValue(char *command)
{
	string *temp = new string(command);
	string *rez = new string(droidSettingsCommandList->getValue(temp));
	delete temp;
	return rez;
}
