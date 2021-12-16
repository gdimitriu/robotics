/*
 * CCommandEsp01.cpp
 *
 *  Created on: Nov 4, 2021
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

#include "CCommandEsp01.h"
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <string.h>

CCommandEsp01::CCommandEsp01(CCommand *move, CCommand *setting) :
		CCommCommands(move, setting) {
	m_serialHandler = -1;
}

CCommandEsp01::~CCommandEsp01() {
	if (m_serialHandler >= 0) {
		serClose(m_serialHandler);
	}
}

void CCommandEsp01::justConsumeData() {
	char buffer[255];
	if (serDataAvailable(m_serialHandler) > 0) {
		memset(buffer, 0, sizeof(buffer));
		int readnr = serRead(m_serialHandler, buffer, 255);
		cout << "from esp<<" << buffer << "<<enddata" << std::endl;
	}
}

void CCommandEsp01::initEsp01() {
	m_serialHandler = serOpen("/dev/ttyS0", 115200, 0);
	if (m_serialHandler < 0) {
		perror("gpio serial open failed");
		gpioTerminate();
		exit(1);
	}
	char message[256];
	sprintf(message, "%s\r\n", "ATE0");
	serWrite(m_serialHandler, message, strlen(message));
	sleep(1);
	justConsumeData();
	memset(message, 0, sizeof(message));
	sprintf(message, "%s\r\n", "AT+CWMODE=3");
	serWrite(m_serialHandler, message, strlen(message));
	sleep(1);
	justConsumeData();
	memset(message, 0, sizeof(message));
	sprintf(message, "%s\r\n", "AT+CIPMUX=1");
	serWrite(m_serialHandler, message, strlen(message));
	sleep(1);
	justConsumeData();
	memset(message, 0, sizeof(message));
	sprintf(message, "%s\r\n", "AT+CIPSERVER=1,8080");
	serWrite(m_serialHandler, message, strlen(message));
	sleep(1);
	justConsumeData();
	memset(message, 0, sizeof(message));
	sprintf(message, "%s\r\n", "AT+CIFSR");
	serWrite(m_serialHandler, message, strlen(message));
	sleep(1);
	justConsumeData();
}

void CCommandEsp01::printMenu() {

}

void CCommandEsp01::startReceiving() {
	char buffer[1024];
	char receiveBuffer[2048];
	string str;
	char sendBuffer[255];
	int recvPosition = 0;
	initEsp01();
	do {
		if (serDataAvailable(m_serialHandler) > 0) {
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(m_serialHandler, buffer, 1024);
			for (int i = 0; i < readnr; i++) {
				if (buffer[i] == '\n' || buffer[i] == '\r') {
					int j;
					int k;
					for (j = i, k = i + 1; j < readnr && k < readnr; k++, j++) {
						buffer[j] = buffer[k];
					}
					for (; j < readnr; j++)
						buffer[j] = '\0';
				}
			}
			buffer[readnr] = '\0';

			if (recvPosition > 0) {
				for (int i = 0; i < readnr; i++, recvPosition++) {
					if (buffer[i] != '\0')
						receiveBuffer[recvPosition] = buffer[i];
					else {
						receiveBuffer[recvPosition] = '\0';
						break;
					}
				}
			} else {
				for (int i = 0; i < readnr; i++, recvPosition++) {
					receiveBuffer[recvPosition] = buffer[i];
					if (buffer[i] == '\0')
						break;
				}
			}
//			cout << "received<<" << receiveBuffer << "<<endreceive"<< std::endl;
			str.assign(receiveBuffer);
			if (str.find('#') != string::npos) {
//				memset(sendBuffer, 0, sizeof(sendBuffer));
//				sprintf(sendBuffer, "AT+CIPCLOSE=0");
//				serWrite(m_serialHandler, sendBuffer, strlen(sendBuffer));
				int commandPos = 0;
				for (int i = 0; i < recvPosition; i++) {
					if (receiveBuffer[i] == ':') {
						commandPos = i + 1;
						break;
					}
				}
				for (int i = commandPos, j = 0; i < recvPosition; i++, j++) {
					if (receiveBuffer[i] == '#') {
						receiveBuffer[j] = receiveBuffer[i];
						receiveBuffer[j + 1] = '\0';
//							memset(&receiveBuffer[j + 1],'\0',sizeof(receiveBuffer)-sizeof(char)*j);
						break;
					}
					receiveBuffer[j] = receiveBuffer[i];
				}
				if (strcmp(receiveBuffer, "exit#") == 0) {
					break;
				}
//				cout << "received: " << receiveBuffer << " endreceive"<< std::endl;
				recvPosition = 0;
				if (receiveBuffer[strlen(receiveBuffer) - 1] == '#') {
					str.assign(receiveBuffer);
//					cout << "command<<" << receiveBuffer << "<<end command"<< std::endl;
					processInputData(&str);
					memset(receiveBuffer, 0, sizeof(receiveBuffer));
				} else {
					cout << "command not<<"
							<< receiveBuffer[strlen(receiveBuffer) - 1]
							<< "<<end command" << std::endl;
				}
			}

		}
	} while (strcmp(receiveBuffer, "exit#") != 0);
}

