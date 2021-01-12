/*
 * CBLECommand.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2021 Gabriel Dimitriu
 * All rights reserved.
 *
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

#include "CBLECommand.h"
#include <pigpio.h>
#include <string.h>
#include <iostream>

CBLECommand::CBLECommand(CCommand *move, CCommand *setting) : CCommCommands(move,setting) {
	m_port = "/dev/rfcomm0";
	m_boudrate = 38400;
	m_serialHandler = serOpen(m_port, m_boudrate, 0);
}

CBLECommand::~CBLECommand() {
	serClose(m_serialHandler);
}

void CBLECommand::printMenu() {
	serWrite(m_serialHandler,"Menu\n",7);
	m_moveCommand->printMenu();
	m_settingCommand->printMenu();
	CCommCommands::printMenu();
}

void CBLECommand::startReceiving() {
	char buffer[255];
	string str;
	do {
		if (serDataAvailable(m_serialHandler) > 0) {
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(m_serialHandler, buffer, 255);
			for(int i = 0; i < readnr; i++) {
				if (buffer[i] == '\n' || buffer[i] == '\r') {
					buffer[i] = '\0';
				}
			}
			if (strcmp(buffer,"exit#") == 0 ) {
				break;
			}
			str.assign(buffer);
			processInputData(&str);
		}
	} while(strcmp(buffer,"exit#") != 0);
}
