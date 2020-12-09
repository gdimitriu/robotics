/*
 * CCommCommands.cpp
 *
 *  Created on: Oct 8, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CCommCommands.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

CCommCommands::CCommCommands(CCommand *move, CCommand *setting) {
	m_moveCommand = move;
	m_settingCommand = setting;
	m_moveOperations = move->getOperationsPrefix();
	m_settingOperations = setting->getOperationsPrefix();
	m_logger = NULL;
}

CCommCommands::~CCommCommands() {
	delete m_moveCommand;
	delete m_settingCommand;
}

void CCommCommands::setDroid(CDroid *droid) {
	m_moveCommand->setDroid(droid);
	m_settingCommand->setDroid(droid);
	m_logger = droid->getLogger();
}

void CCommCommands::processInputData(string *input) {
	if (m_moveOperations->find(input->at(0)) != m_moveOperations->end()) {
		m_moveCommand->execute(input->c_str());
	} else if (m_settingOperations->find(input->at(0)) != m_settingOperations->end()) {
		m_settingCommand->execute(input->c_str());
	} else {
		char message[255];
		memset(message,0,255);
		sprintf(message,"Operation not known = %s\n", input->c_str());
		m_logger->error(message);
	}
}

