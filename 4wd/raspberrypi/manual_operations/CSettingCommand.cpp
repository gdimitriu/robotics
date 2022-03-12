/*
 * CSettingCommand.cpp
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

#include "CSettingCommand.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>

CSettingCommand::CSettingCommand() {
	init();
}

CSettingCommand::CSettingCommand(CLogger *logger) :
		CCommand(logger) {
	init();
}

void CSettingCommand::init() {
	m_operations = new set<char>();
	m_menu = new string();
	m_operations->insert('V');
	m_operations->insert('v');
	m_operations->insert('d');
	m_operations->insert('s');
	m_operations->insert('c');
	m_operations->insert('I');
	m_menu->append("c# print current power\t;\n");
	m_menu->append("cxxx# change current power to xxx\t;\n");
	m_menu->append("V# print max power\t;\n");
	m_menu->append("Vxxx# change max power to xxx\t;\n");
	m_menu->append("v# print min power\t;\n");
	m_menu->append("vxxx# change min power to xxx\t;\n");
	m_menu->append("d# print low power distance\t;\n");
	m_menu->append("dxxx# change low power distance to xxx\t;\n");
	m_menu->append("s# print stop distance\t;\n");
	m_menu->append("sxxx# change stop distance to xxx\t;\n");
	m_menu->append("I# get connection and capability informations\t;\n");
	m_droid = NULL;
}

CSettingCommand::~CSettingCommand() {
	m_operations->clear();
	delete m_operations;
	delete m_menu;
}

void CSettingCommand::setDroid(CDroid *droid) {
	m_droid = droid;
	if (m_logger == NULL)
		m_logger = droid->getLogger();
}

set<char>* CSettingCommand::getOperationsPrefix() {
	return m_operations;
}

void CSettingCommand::printMenu() {
	m_logger->info(*m_menu);
}

int CSettingCommand::executeSimpleCommand(char *operation) {
	char message[REPLY_MESSAGE_LENGTH];
	memset(message, 0, REPLY_MESSAGE_LENGTH * sizeof(char));
	switch (operation[0]) {
	case 'V':
		sprintf(message, "max power %u\n", m_droid->getMaxEnginePower());
		m_logger->info(message);
		sprintf(message, "%u", m_droid->getMaxEnginePower());
		setRepliedMessage(message);
		return 1;
	case 'v':
		sprintf(message, "min power %u\n", m_droid->getMinEnginePower());
		m_logger->info(message);
		sprintf(message, "%u", m_droid->getMinEnginePower());
		setRepliedMessage(message);
		return 1;
	case 'c':
		sprintf(message, "current power %u\n",
				m_droid->getCurrentEnginePower());
		m_logger->info(message);
		sprintf(message, "%u", m_droid->getCurrentEnginePower());
		setRepliedMessage(message);
		return 1;
	case 'd':
		sprintf(message, "min power distance %lu\n",
				m_droid->getLowPowerDistance());
		m_logger->info(message);
		sprintf(message, "%lu", m_droid->getLowPowerDistance());
		setRepliedMessage(message);
		return 1;
	case 's':
		sprintf(message, "stop distance %lu\n", m_droid->getStopDistance());
		m_logger->info(message);
		sprintf(message, "%lu", m_droid->getStopDistance());
		setRepliedMessage(message);
		return 1;
	case 'I':
		createInfoMessage(message);
		setRepliedMessage(message);
		return 1;
	default:
		sprintf(message, "Invalid settings with data command = %s\n",
				operation);
		m_logger->error(message);
		return 1;
	}
	return 0;
}

int CSettingCommand::executeDataCommand(char *operation) {
	unsigned int uintValue;
	unsigned long ulongValue;
	char message[REPLY_MESSAGE_LENGTH];
	memset(message, 0, REPLY_MESSAGE_LENGTH * sizeof(char));
	switch (operation[0]) {
	case 'V':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "max power changed from %u to %u\n",
					m_droid->getMaxEnginePower(), uintValue);
			m_logger->debug(message);
		}
		m_droid->setMaxEnginePower(uintValue);
		sprintf(message, "OK");
		setRepliedMessage(message);
		return 1;
	case 'v':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "min power changed from %u to %u\n",
					m_droid->getMinEnginePower(), uintValue);
			m_logger->debug(message);
		}
		m_droid->setMinEnginePower(uintValue);
		sprintf(message, "OK");
		setRepliedMessage(message);
		return 1;
	case 'c':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "current power changed from %u to %u\n",
					m_droid->getCurrentEnginePower(), uintValue);
			m_logger->debug(message);
		}
		m_droid->setCurrentEnginePower(uintValue);
		sprintf(message, "OK");
		setRepliedMessage(message);
		return 1;
	case 'd':
		removeCommandPrefix(operation);
		sscanf(operation, "%lu", &ulongValue);
		if (m_logger->isDebug()) {
			sprintf(message, "min power distance changed from %lu to %lu\n",
					m_droid->getLowPowerDistance(), ulongValue);
			m_logger->debug(message);
		}
		m_droid->setLowPowerDistance(ulongValue);
		sprintf(message, "OK");
		setRepliedMessage(message);
		return 1;
	case 's':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "stop distance changed from %u to %u\n",
					m_droid->getStopDistance(), uintValue);
			m_logger->debug(message);
		}
		m_droid->setStopDistance(uintValue);
		sprintf(message, "OK");
		setRepliedMessage(message);
		return 1;
	default:
		sprintf(message, "Invalid settings with data command = %s\n",
				operation);
		m_logger->error(message);
		return -1;
	}
	return 0;
}

int CSettingCommand::createInfoMessage(char *message) {
	std::string *info = new string("wifi:");
	struct ifaddrs *addrs, *tmp;
	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp) {
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
			if(strcmp(tmp->ifa_name,"lo") != 0) {
				*info += inet_ntoa(((struct sockaddr_in*) tmp->ifa_addr)->sin_addr);
				*info += ",";
			}
		}
		tmp = tmp->ifa_next;
	}
	info->pop_back();
	*info += "#";
	std::string *droid = m_droid->getInfo();
	*info += *droid;
	delete droid;
	strncpy(message, info->c_str(), info->length());
	delete info;
	return 1;
}

