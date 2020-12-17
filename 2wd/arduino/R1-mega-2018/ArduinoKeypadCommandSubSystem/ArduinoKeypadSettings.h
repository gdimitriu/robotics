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
#ifndef ArduinoKeypadSettings_h
#define ArduinoKeypadSettings_h

class ArduinoKeypadSettings
{
public:
	ArduinoKeypadSettings(int row, int cols);
	void setKeyMap(char *keys);
	char * getKeyMap();
	void setRowPins(char *pins);
	char * getRowPins();
	void setColPins(char *pins);
	char * getColPins();
	int getNrRows();
	int getNrColumns();
	void setCommandButton(int button);
	int getCommandButton();
private:
	int nrRows;
	int nrColumns;
	char *keyMap;
	char *rowPins;
	char *colPins;
	int commandButton;
};

#endif