/*
 * CHttpCommand.cpp
 *
 *  Created on: Feb 25, 2022
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2022 Gabriel Dimitriu
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

#include "CHttpCommand.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <errno.h>

extern int errno;

CHttpCommand::CHttpCommand(CCommand *move, CCommand *setting) : CCommCommands(move, setting){
	m_listenSocketFd = -1;
	bzero(&m_clientAddr, sizeof(m_clientAddr));
	bzero(&m_serverAddr, sizeof(m_serverAddr));
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_httpPort = HTTP_PORT;
	m_serverAddr.sin_port = htons(m_httpPort);
	m_logger = setting->getLogger();
	m_clients = (int *)calloc(MAX_NR_CLIENTS,sizeof(int));
	m_buffer = (char **)calloc(MAX_NR_CLIENTS, sizeof(char *));
	for (int i = 0; i < MAX_NR_CLIENTS; i++) {
		m_buffer[i] = (char *)calloc(RECEIVE_FULL_BUF_LEN, sizeof(char));
	}
	m_possionInBuffer = (int *) calloc(MAX_NR_CLIENTS,sizeof(int));
}

CHttpCommand::~CHttpCommand() {
	if (m_listenSocketFd > 0) {
		if (m_listenSocketFd > 0)
			close(m_listenSocketFd);
		m_listenSocketFd = -1;
	}
	if (m_clients != NULL) {
		for (int i = 0; i < MAX_NR_CLIENTS; i++) {
			if (m_clients[i] > 0)
				close(m_clients[i]);
		}
		free(m_clients);
	}
	if (m_buffer != NULL) {
		for (int i = 0 ;i < MAX_NR_CLIENTS; i++)
			free(m_buffer[i]);
		free(m_buffer);
	}
	if (m_possionInBuffer != NULL)
		free(m_possionInBuffer);
}

void CHttpCommand::printMenu() {

}

void CHttpCommand::startReceiving() {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	if ((m_listenSocketFd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		char message[255];
		sprintf(message,"Could not create listen Socket: %s\n",strerror(errno));
		m_logger->error(message);
		return;
	}
	if (bind(m_listenSocketFd, (struct sockaddr*) &m_serverAddr, sizeof(m_serverAddr)) < 0) {
		char message[255];
		sprintf(message,"Could not bind Socket: %s\n",strerror(errno));
		m_logger->error(message);
		return;
	}
	if (listen(m_listenSocketFd,0) < 0) {
		char message[255];
		sprintf(message,"Could not listen Socket: %s\n",strerror(errno));
		m_logger->error(message);
		return;
	}
	int nready;
	int maxfd,connectSocketFd;
	int maxi;
	int i,n;
	socklen_t clilen;
	string str;
	int receiveNr;
	char receiveBuffer[RECEIVE_BUF_LEN];
	maxfd = m_listenSocketFd;
	maxi = -1;
	for (i = 0; i < MAX_NR_CLIENTS; i++)
		m_clients[i] = -1;
	FD_ZERO(&m_allset);
	FD_SET(m_listenSocketFd,&m_allset);
	struct timeval timeout;
	while(!isStopped()) {
		pthread_testcancel();
		m_rset = m_allset;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		nready = select(maxfd + 1, &m_rset, NULL, NULL, &timeout);
		if (nready == 0) {
			continue;
		}
		if (isStopped())
			return;
		if (FD_ISSET(m_listenSocketFd,&m_rset)) { //new client
			clilen = sizeof(m_clientAddr);
			connectSocketFd = accept(m_listenSocketFd,(struct sockaddr *)&m_clientAddr, &clilen);
			if (connectSocketFd < 0) {
				char message[255];
				sprintf(message,"Could not accept Socket: %s\n",strerror(errno));
				m_logger->error(message);
				continue;
			}
			for (i = 0; i < MAX_NR_CLIENTS; i++)
				if (m_clients[i] < 0) {
					m_clients[i] = connectSocketFd;
					break;
				}
			if (i == MAX_NR_CLIENTS) {
				char message[255];
				sprintf(message,"Too many clients: %s\n",strerror(errno));
				m_logger->error(message);
				close(connectSocketFd);
				continue;
			}
			FD_SET(connectSocketFd,&m_allset); //add new descriptor
			int one = 1;
			setsockopt(connectSocketFd, SOL_TCP, TCP_NODELAY, &one, sizeof(one));
			if (connectSocketFd > maxfd)
				maxfd = connectSocketFd;
			if (i > maxi)
					maxi = i;
			if (--nready <= 0)
				continue;
		}
		for (int clientId = 0;clientId <= maxi; clientId++) {
			if ((connectSocketFd = m_clients[clientId]) < 0)
				continue;
			if (FD_ISSET(connectSocketFd,&m_rset)) {
				bzero(receiveBuffer, sizeof(char)*RECEIVE_BUF_LEN);
				receiveNr = read(connectSocketFd,receiveBuffer, RECEIVE_BUF_LEN);
				if (receiveNr == 0) {
					close(connectSocketFd);
					m_possionInBuffer[clientId] = 0;
					bzero(m_buffer[clientId],sizeof(char) * RECEIVE_FULL_BUF_LEN);
					m_clients[clientId] = -1;
					FD_CLR(connectSocketFd,&m_allset);
				}else {
					if (m_possionInBuffer[clientId] == 0) {
						for(int i = 0; i < receiveNr; i++) {
							m_buffer[clientId][i] = receiveBuffer[i];
						}
						receiveBuffer[receiveNr] = '\0';
						m_possionInBuffer[clientId] = receiveNr;
					} else {
						for (int i = 0; i <= receiveNr; i++, m_possionInBuffer[clientId]++) {
							m_buffer[clientId][m_possionInBuffer[clientId]] = receiveBuffer[i];
						}
					}
					str.assign(m_buffer[clientId]);
					if (str.find('#') != string::npos) {
						vector<string *> *commands = splitCommandsAndClearBuffer(m_buffer[clientId],&m_possionInBuffer[clientId],RECEIVE_FULL_BUF_LEN);
						for(vector<string *>::iterator it = commands->begin(); it != commands->end(); ++it) {
							if (string("exit#").compare(**it) == 0) {
								stop();
								freeCommands(commands);
								FD_CLR(connectSocketFd,&m_allset);
								delete commands;
								return;
							}
							int ret = processInputData(*it);
							if(ret == 1 || m_hasAck) {
								sendData(clientId);
							}
						}
						freeCommands(commands);
						delete commands;
					}
					if (--nready <= 0)
						break;
				}
			}
		}
	}
}

void CHttpCommand::freeCommands(vector<string *> *commands) {
	for(vector<string *>::iterator it = commands->begin(); it != commands->end(); ++it) {
		delete *it;
	}
}
void CHttpCommand::sendData(int clientId) {
	char sendBuffer[SEND_BUF_LEN];
	if (!m_hasAck) {
		char *message = getSettingCommand()->getRepliedMessage();
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer,"%s\n",message);
		write(m_clients[clientId],sendBuffer,strlen(sendBuffer));
	} else {
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer,"OK\n");
		write(m_clients[clientId],sendBuffer,strlen(sendBuffer));
		m_hasAck = false;
	}
}
