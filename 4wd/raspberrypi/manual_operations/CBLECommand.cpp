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
#include <fcntl.h>

CBLECommand::CBLECommand(CCommand *move, CCommand *setting) :
		CCommCommands(move, setting) {
	m_port = "/dev/rfcomm0";
	m_boudrate = 38400;
	m_serialHandler = -1;
}

CBLECommand::~CBLECommand() {
	if (m_serialHandler >= 0) {
		serClose(m_serialHandler);
	}
}

void CBLECommand::printMenu() {
	serWrite(m_serialHandler, "Menu\n", 7);
	m_moveCommand->printMenu();
	m_settingCommand->printMenu();
	CCommCommands::printMenu();
}

void CBLECommand::startReceiving() {
	char buffer[255];
	string str;
	char sendBuffer[255];
	int fd = -1;
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	while(fd < 0) {
		fd = open(m_port,O_RDWR);
		if (fd < 0) {
			if (isStopped())
				return;
			sleep(1);
		}
	}
	close(fd);
	m_serialHandler = serOpen(m_port, m_boudrate, 0);
	do {
		if (serDataAvailable(m_serialHandler) > 0) {
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(m_serialHandler, buffer, 255);
			for (int i = 0; i < readnr; i++) {
				if (buffer[i] == '\n' || buffer[i] == '\r') {
					buffer[i] = '\0';
				}
			}
			if (strcmp(buffer, "exit#") == 0) {
				stop();
				break;
			}
			str.assign(buffer);
			int ret = processInputData(&str);
			if (ret == 1) {
				char *message = getSettingCommand()->getRepliedMessage();
				memset(sendBuffer, 0, sizeof(sendBuffer));
				sprintf(sendBuffer,"%s\r\n",message);
				serWrite(m_serialHandler,sendBuffer,strlen(sendBuffer));
			} else {
				if (m_hasAck) {
					memset(sendBuffer, 0, sizeof(sendBuffer));
					sprintf(sendBuffer,"OK\r\n");
					serWrite(m_serialHandler,sendBuffer,strlen(sendBuffer));
					m_hasAck = false;
				}
			}
		}
		pthread_testcancel();
	} while ((strcmp(buffer, "exit#") != 0) && !isStopped());
}
