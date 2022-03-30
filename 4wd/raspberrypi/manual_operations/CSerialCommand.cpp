/*
 * CSerialCommand.cpp
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

#include "CSerialCommand.h"
#include <pigpio.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>

CSerialCommand::CSerialCommand(CCommand *move, CCommand *setting) :
		CCommCommands(move, setting) {
	m_port = "/dev/ttyS0";
	m_boudrate = 115200;
	m_serialHandler = -1;
}

CSerialCommand::~CSerialCommand() {
	if (m_serialHandler >= 0) {
		serClose(m_serialHandler);
	}
}

void CSerialCommand::printMenu() {
	serWrite(m_serialHandler, "Menu\n", 7);
	m_moveCommand->printMenu();
	m_settingCommand->printMenu();
	CCommCommands::printMenu();
}

void CSerialCommand::startReceiving() {
	char buffer[RECEIVE_FULL_BUF_LEN];
	char receiveBuffer[RECEIVE_BUF_LEN];
	string str;
	int fd = -1;
	int bufferPosition = 0;
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
			memset(receiveBuffer, 0, sizeof(receiveBuffer));
			int readnr = serRead(m_serialHandler, receiveBuffer, RECEIVE_BUF_LEN);
			for (int i = 0; i < readnr; i++) {
				//commands does not have terminators
				if(receiveBuffer[i] == '\n' || receiveBuffer[i] == '\r' || receiveBuffer[i] == ' ' || receiveBuffer[i] == '\t' || receiveBuffer[i] == '\0' || receiveBuffer[i] < 35 || receiveBuffer[i] > 122) {
					receiveBuffer[i] = '\0';
				}
			}
			int j = 0;
			for (int i = 0; i < readnr; i++) {
				if (receiveBuffer[i] == '\0') {
					continue;
				} else {
					receiveBuffer[j] = receiveBuffer[i];
					j++;
				}

			}
			receiveBuffer[j] = '\0';
			if (bufferPosition == 0) {
				memset(buffer, '\0', sizeof(buffer));
				for (int i = 0 ;i < strlen(receiveBuffer); i++, bufferPosition++) {
					buffer[i] = receiveBuffer[i];
				}
			} else {
				for (int i = 0 ;i < strlen(receiveBuffer); i++, bufferPosition++) {
					buffer[bufferPosition] = receiveBuffer[i];
				}
			}
			str.assign(buffer);
			if (str.find('#') != string::npos) {
				vector<string *> *commands = splitCommandsAndClearBuffer(buffer,&bufferPosition,RECEIVE_FULL_BUF_LEN);
				for(vector<string *>::iterator it = commands->begin(); it != commands->end(); ++it) {
					if (string("exit#").compare(**it) == 0) {
						stop();
						freeCommands(commands);
						delete commands;
						return;
					}
					int ret = processInputData(*it);
					if(ret == 1 || m_hasAck) {
						sendData();
					}
				}
				freeCommands(commands);
				delete commands;
			}
		}
		pthread_testcancel();
	} while (!isStopped());
}

void CSerialCommand::freeCommands(vector<string *> *commands) {
	for(vector<string *>::iterator it = commands->begin(); it != commands->end(); ++it) {
		delete *it;
	}
}

void CSerialCommand::sendData() {
	char sendBuffer[256];
	if (!m_hasAck) {
		char *message = getSettingCommand()->getRepliedMessage();
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer,"%s\r\n",message);
		serWrite(m_serialHandler,sendBuffer,strlen(sendBuffer));
	} else {
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer,"OK\r\n");
		serWrite(m_serialHandler,sendBuffer,strlen(sendBuffer));
		m_hasAck = false;
	}
}
