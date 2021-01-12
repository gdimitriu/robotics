/*
 * CCommCommands.cpp
 *
 *  Created on: Oct 2, 2020
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
	m_menu = new string();
	m_menu->append("Rs# start recording of movement\t;\n");
	m_menu->append("Rc# continue old recording of movement\t;\n");
	m_menu->append("RS# stop recording of movement\t;\n");
	m_menu->append("P# play the record of movement\t;\n");
	m_isRecording = false;
	m_localOperations.insert('R'); //start/stop recording of movement
	m_localOperations.insert('P'); //play the recording of movement
	m_droid = NULL;
}

CCommCommands::~CCommCommands() {
	delete m_moveCommand;
	delete m_settingCommand;
	delete m_menu;
}

void CCommCommands::printMenu() {
	m_logger->info(*m_menu);
}

void CCommCommands::setDroid(CDroid *droid) {
	m_moveCommand->setDroid(droid);
	m_settingCommand->setDroid(droid);
	m_logger = droid->getLogger();
	m_droid = droid;
}

void CCommCommands::processInputData(string *input) {
	int status;
	string *command;
	if (m_moveOperations->find(input->at(0)) != m_moveOperations->end()) {
		status = m_moveCommand->execute(input->c_str());
	} else if (m_settingOperations->find(input->at(0)) != m_settingOperations->end()) {
		status = m_settingCommand->execute(input->c_str());
	} else if (m_localOperations.find(input->at(0)) != m_localOperations.end()) {
		executeLocal(input->c_str());
		return;
	} else {
		char message[255];
		memset(message,0,255);
		sprintf(message,"Operation not known = %s\n", input->c_str());
		m_logger->error(message);
		return;
	}
	if (status == 0 && m_isRecording) {
		command = new string(*input);
		m_recordMovement.push_back(command);
	}
}

int CCommCommands::executeLocal(const char *operation) {
	if (operation == NULL) {
		return 0;
	}
	int result = 0;
	char message[255];
	memset(message, 0, 255 * sizeof(char));
	switch (operation[0]) {
	case 'P':
		m_droid->setBlockingOperation(true);
		for (list<string *>::iterator in= m_recordMovement.begin(); in != m_recordMovement.end();in++) {
			processInputData(*in);
		}
		m_droid->setBlockingOperation(false);
		break;
	case 'R':
		if (strlen(operation) != 3) {
			sprintf(message, "Invalid Command with data command = %s\n", operation);
			m_logger->error(message);
			return 1;
		}
		if (operation[1] == 's') {
			for (list<string *>::iterator in= m_recordMovement.begin(); in != m_recordMovement.end();in++) {
				delete *in;
			}
			m_recordMovement.clear();
			m_isRecording = true;
			return 0;
		}
		if (operation[1] == 'S') {
			m_isRecording = false;
			return 0;
		}
		if (operation[1] == 'c') {
			m_isRecording = true;
			return 0;
		}
		sprintf(message, "Invalid Command with data command = %s\n", operation);
		m_logger->error(message);
		return 1;
	default:
		sprintf(message, "Invalid Command with data command = %s\n", operation);
		m_logger->error(message);
		return 1;
	}
	return result;
}
