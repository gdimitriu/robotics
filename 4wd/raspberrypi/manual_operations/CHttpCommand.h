/*
 * CHttpCommand.h
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

#ifndef MANUAL_OPERATIONS_CHTTPCOMMAND_H_
#define MANUAL_OPERATIONS_CHTTPCOMMAND_H_

#include "CCommCommands.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <CLogger.h>
#define HTTP_PORT 8080
#define MAX_NR_CLIENTS 2
#define RECEIVE_FULL_BUF_LEN 2048
#define RECEIVE_BUF_LEN RECEIVE_FULL_BUF_LEN/2
#define SEND_BUF_LEN 256

class CHttpCommand: public CCommCommands {
public:
	CHttpCommand(CCommand *move, CCommand *setting);
	virtual ~CHttpCommand();
	virtual void startReceiving();
	virtual void printMenu();
private:
	void sendData(int clientId);
	void freeCommands(vector<string *> *commands);
	int m_listenSocketFd;
	struct sockaddr_in m_clientAddr;
	struct sockaddr_in m_serverAddr;
	int m_httpPort;
	CLogger *m_logger;
	fd_set m_rset;
	fd_set m_allset;
	int *m_clients;
	char **m_buffer;
	int *m_possionInBuffer;
};

#endif /* MANUAL_OPERATIONS_CHTTPCOMMAND_H_ */
