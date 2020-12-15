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
	m_menu->append("V# print max power\t;\n");
	m_menu->append("Vxxx# change max power to xxx\t;\n");
	m_menu->append("v# print min power\t;\n");
	m_menu->append("vxxx# change min power to xxx\t;\n");
	m_menu->append("d# print low power distance\t;\n");
	m_menu->append("dxxx# change low power distance to xxx\t;\n");
	m_menu->append("s# print stop distance\t;\n");
	m_menu->append("sxxx# change stop distance to xxx\t;\n");
	m_droid = NULL;
}

CSettingCommand::~CSettingCommand() {
	m_operations->clear();
	delete m_operations;
	delete m_menu;
}

void CSettingCommand::setDroid(CDroid *droid) {
	m_droid = droid;
	m_logger = droid->getLogger();
}

set<char>* CSettingCommand::getOperationsPrefix() {
	return m_operations;
}

void CSettingCommand::printMenu() {
	m_logger->info(*m_menu);
}

int CSettingCommand::executeSimpleCommand(char *operation) {
	char message[255];
	memset(message, 0, 255 * sizeof(char));
	switch (operation[0]) {
	case 'V':
		removeCommandPrefix(operation);
		sprintf(message, "max power %u\n",m_droid->getMaxEnginePower());
		m_logger->info(message);
		break;
	case 'v':
		removeCommandPrefix(operation);
		sprintf(message, "min power %u\n",m_droid->getMinEnginePower());
		m_logger->info(message);
		break;
	case 'd':
		removeCommandPrefix(operation);
		sprintf(message, "min power distance %lu\n",m_droid->getLowPowerDistance());
		m_logger->info(message);
		break;
	case 's':
		removeCommandPrefix(operation);
		sprintf(message, "stop distance %lu\n",m_droid->getStopDistance());
		m_logger->info(message);
		break;
	default:
		sprintf(message, "Invalid settings with data command = %s\n", operation);
		m_logger->error(message);
		return 1;
	}
	return 0;
}

int CSettingCommand::executeDataCommand(char *operation) {
	unsigned int uintValue;
	unsigned long ulongValue;
	char message[255];
	memset(message, 0, 255 * sizeof(char));
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
		break;
	case 'v':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "min power changed from %u to %u\n",
					m_droid->getMinEnginePower(), uintValue);
			m_logger->debug(message);
		}
		m_droid->setMinEnginePower(uintValue);
		break;
	case 'd':
		removeCommandPrefix(operation);
		sscanf(operation, "%lu", &ulongValue);
		if (m_logger->isDebug()) {
			sprintf(message, "min power distance changed from %lu to %lu\n",
					m_droid->getLowPowerDistance(), ulongValue);
			m_logger->debug(message);
		}
		m_droid->setLowPowerDistance(ulongValue);
		break;
	case 's':
		removeCommandPrefix(operation);
		sscanf(operation, "%u", &uintValue);
		if (m_logger->isDebug()) {
			sprintf(message, "stop distance changed from %u to %u\n",
					m_droid->getStopDistance(), uintValue);
			m_logger->debug(message);
		}
			m_droid->setStopDistance(uintValue);
			break;
	default:
		sprintf(message, "Invalid settings with data command = %s\n", operation);
		m_logger->error(message);
		return 1;
	}
	return 0;
}

