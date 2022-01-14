/*
 * CBLECommandHC05.cpp
 *
 *  Created on: Oct 5, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
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

#include "CBLECommandHC05.h"
#include <pigpio.h>
#include <string.h>
#include <iostream>

CBLECommandHC05::CBLECommandHC05(CCommand *move, CCommand *setting) : CCommCommands(move,setting) {
	m_port = "/dev/ttyAMA0";
	m_boudrate = 38400;
	m_serialHandler = serOpen(m_port, m_boudrate, 0);
}

CBLECommandHC05::~CBLECommandHC05() {
	serClose(m_serialHandler);
}

void CBLECommandHC05::printMenu() {
	serWrite(m_serialHandler,"Menu\n",7);
	m_moveCommand->printMenu();
	m_settingCommand->printMenu();
}

void CBLECommandHC05::startReceiving() {
	char buffer[255];
	string str;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
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
				stop();
				break;
			}
			str.assign(buffer);
			processInputData(&str);
		}
		pthread_testcancel();
	} while((strcmp(buffer,"exit#") != 0) && !isStopped());
}
