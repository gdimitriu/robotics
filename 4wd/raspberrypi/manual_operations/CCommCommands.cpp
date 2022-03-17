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
#include <iostream>
#include <fstream>

bool CCommCommands::m_isStopped = false;

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
	m_menu->append("Pr# play the record of movement in reverse order only left right\t;\n");
	m_menu->append("PR# play the record of movement in full reverse order\t;\n");
	m_menu->append("S# save record into the file\t;\n");
	m_menu->append("LfilePath# load record from the file\t;\n");
	m_menu->append("T# Start streaming camera\t;\n");
	m_menu->append("t# Stop streaming camera\t;\n");
	m_menu->append("Go/c# grab the object using claw (o for open c for close)\t;\n");
	m_menu->append("p# print the menu\t;\n");
	m_isRecording = false;
	m_localOperations.insert('R'); //start/stop recording of movement
	m_localOperations.insert('P'); //play the recording of movement or play in reverse the recording
	m_localOperations.insert('T'); //start streaming camera
	m_localOperations.insert('t'); //stop streaming camera
	m_localOperations.insert('S'); //save record as file
	m_localOperations.insert('L'); //load record file
	m_localOperations.insert('G'); //grab
	m_localOperations.insert('p'); //print menu
	m_droid = NULL;
	m_isStopped = false;
	m_hasAck = false;
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

int CCommCommands::processInputData(string *input, int reverse) {
	int status;
	if (input->empty() || input->size() == 1)
		return 0;
	if (input->back() != '#') {
		char message[255];
		memset(message,0,255);
		sprintf(message,"Operation not known because is not end with # =<<%s<<END\n", input->c_str());
		m_logger->error(message);
		return 0;
	}
	string *command;
	m_moveCommand->setReverseType(reverse);
	if (m_moveOperations->find(input->at(0)) != m_moveOperations->end()) {
		status = m_moveCommand->execute(input->c_str());
		return status;
	} else if (m_settingOperations->find(input->at(0)) != m_settingOperations->end()) {
		status = m_settingCommand->execute(input->c_str());
		return status;
	} else if (m_localOperations.find(input->at(0)) != m_localOperations.end()) {
		executeLocal(input->c_str());
		return 0;
	} else {
		char message[255];
		memset(message,0,255);
		sprintf(message,"Operation not known = %s\n", input->c_str());
		m_logger->error(message);
		m_moveCommand->setReverseType(0);
		return 0;
	}
	if (status == 0 && m_isRecording) {
		command = new string(*input);
		m_recordMovement.push_back(command);
	}
	return 0;
}

int CCommCommands::executeLocal(const char *operation) {
	if (operation == NULL) {
		return 0;
	}
	std::ofstream *pFile;
	int result = 0;
	char message[255];
	memset(message, 0, 255 * sizeof(char));
	char * newOperation;
	switch (operation[0]) {
	case 'T':
		newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
		strncpy(newOperation, operation, strlen(operation));
		removeCommandPrefix(newOperation);
		//clear the #
		newOperation[strlen(newOperation) -1] = '\0';
		m_droid->startStreaming();
		free(newOperation);
		m_hasAck = true;
		return 0;
	case 't':
		newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
		strncpy(newOperation, operation, strlen(operation));
		removeCommandPrefix(newOperation);
		//clear the #
		newOperation[strlen(newOperation) -1] = '\0';
		m_droid->stopStreaming();
		free(newOperation);
		m_hasAck = false;
		return 0;
	case 'P':
			newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
			strncpy(newOperation, operation, strlen(operation));
			removeCommandPrefix(newOperation);
			//clear the #
			newOperation[strlen(newOperation) -1] = '\0';
			if (strlen(newOperation)==0) {
				m_droid->setBlockingOperation(true);
				for (vector<string *>::iterator in= m_recordMovement.begin(); in != m_recordMovement.end();in++) {
					processInputData(*in);
				}
				m_droid->setBlockingOperation(false);
			} else if (strlen(newOperation) == 1 && newOperation[0] == 'r') {
				m_droid->setBlockingOperation(true);
				//rotate 180 degree
				string *pstr = new string("m0,1#");
				processInputData(pstr);
				processInputData(pstr);
				for (vector<string *>::reverse_iterator in= m_recordMovement.rbegin(); in != m_recordMovement.rend();in++) {
					processInputData(*in, 1);
				}
				processInputData(pstr);
				processInputData(pstr);
				delete pstr;
				m_droid->setBlockingOperation(false);
			} else if (strlen(newOperation) == 1 && newOperation[0] == 'R') {
				m_droid->setBlockingOperation(true);
				for (vector<string *>::reverse_iterator in= m_recordMovement.rbegin(); in != m_recordMovement.rend();in++) {
					processInputData(*in, 2);
				}
				m_droid->setBlockingOperation(false);
			}
			free(newOperation);
			m_hasAck = false;
			break;
	case 'R':
		if (strlen(operation) != 3) {
			sprintf(message, "Invalid Command with data command = %s\n", operation);
			m_logger->error(message);
			m_hasAck = false;
			return 1;
		}
		if (operation[1] == 's') {
			for (vector<string *>::iterator in= m_recordMovement.begin(); in != m_recordMovement.end();in++) {
				delete *in;
			}
			m_recordMovement.clear();
			m_isRecording = true;
			m_hasAck = false;
			return 0;
		}
		if (operation[1] == 'S') {
			m_isRecording = false;
			m_hasAck = false;
			return 0;
		}
		if (operation[1] == 'c') {
			m_isRecording = true;
			m_hasAck = false;
			return 0;
		}
		sprintf(message, "Invalid Command with data command = %s\n", operation);
		m_logger->error(message);
		m_hasAck = false;
		return 1;
	case 'S': {
		cout<<"Receive S but ... "<<operation<<endl<<flush;
		newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
		strncpy(newOperation, operation, strlen(operation));
		removeCommandPrefix(newOperation);
		//clear the #
		newOperation[strlen(newOperation) -1] = '\0';
		pFile = new std::ofstream(newOperation,std::ios::out | std::ios::app | std::ios::ate);
		if (!pFile->good()) {
			if (pFile->is_open())
				pFile->close();
			sprintf(message, "Invalid filename for saving = %s\n", newOperation);
			m_logger->error(message);
			free(newOperation);
			m_hasAck = false;
			return 1;
		}
		for (int i = 0; i < (m_recordMovement.size() - 1); i++) {
			(*pFile)<<*m_recordMovement.at(i)<<std::endl<<flush;
		}
		(*pFile)<<*m_recordMovement.at(m_recordMovement.size() - 1)<<flush;
		pFile->close();
		free(newOperation);
		delete pFile;
		break;
	}
	case 'L': {
		for (vector<string *>::iterator in= m_recordMovement.begin(); in != m_recordMovement.end();in++) {
			delete *in;
		}
		m_recordMovement.clear();
		char * newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
		strcpy(newOperation, operation);
		removeCommandPrefix(newOperation);
		//clear the #
		newOperation[strlen(newOperation) -1] = '\0';
		char *buffer = (char *)calloc(256, sizeof(char));
		std::ifstream *pFile = new std::ifstream(newOperation);
		if (!pFile->good()) {
			if (pFile->is_open())
				pFile->close();
			sprintf(message, "Invalid filename for loading = %s\n", newOperation);
			m_logger->error(message);
			m_hasAck = false;
			return 1;
		}
		while(!pFile->eof()) {
			pFile->getline(buffer, 256, '\n');
			m_recordMovement.push_back(new string(buffer));
		}
		pFile->close();
		free(newOperation);
		break;
	}
	case 'G':
		newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
		strncpy(newOperation, operation, strlen(operation));
		removeCommandPrefix(newOperation);
		//clear the #
		newOperation[strlen(newOperation) -1] = '\0';
		if (strlen(newOperation) == 0) {
			sprintf(message, "Invalid Command with data command = %s\n", operation);
			m_logger->error(message);
			m_hasAck = false;
			return 1;
		}
		if (newOperation[0] == 'o') {
			m_droid->openClaw();
		} else if (newOperation[0] == 'c') {
			m_droid->closeClaw();
		}
		free(newOperation);
		break;
	case 'p':
		m_moveCommand->printMenu();
		m_settingCommand->printMenu();
		printMenu();
		m_hasAck = false;
		break;
	default:
		sprintf(message, "Invalid Command with data command = %s\n", operation);
		m_logger->error(message);
		m_hasAck = false;
		return 0;
	}
	m_hasAck = false;
	return result;
}

void CCommCommands::removeCommandPrefix(char *operation) {
	int size = strlen(operation);
	for (int i = 0; i < size; i++) {
		operation[i] = operation[i+1];
	}
	operation[size] = '\0';
}

CCommand* CCommCommands::getSettingCommand() {
	return m_settingCommand;
}

void CCommCommands::stop() {
	m_isStopped = true;
}

int CCommCommands::isStopped() {
	return m_isStopped;
}

vector<string *> *CCommCommands::splitCommandsAndClearBuffer(char *buffer,int *index,int maxSize) {
	vector<string *> *commands = new vector<string *>();
	char tmpBuffer[1024];
	bzero(tmpBuffer,sizeof(char)*1024);
	int commPos = 0;
	int lastCommand = 0;
	int actualSize = strlen(buffer);
	for (int i = 0 ;i < actualSize; i++) {
		if (buffer[i]== '#') {
			tmpBuffer[commPos] = buffer[i];
			tmpBuffer[commPos + 1] = '\0';
			commands->push_back(new string(tmpBuffer));
			bzero(tmpBuffer,sizeof(char)*1024);
			commPos = 0;
			lastCommand = i+1;
		} else {
			if (!(buffer[i] == '\n' || buffer[i] =='\r' || buffer[i] ==' ' || buffer[i] =='\t')) {
				tmpBuffer[commPos] = buffer[i];
				commPos++;
			}
		}
		(*index)--;
	}
	bzero(buffer,sizeof(char)*maxSize);
	if (strlen(tmpBuffer) > 0) {
		cout<<"Has more <<"<<tmpBuffer<<"<<END"<<std::endl;
		for(int i = 0; i< commPos; i++) {
			buffer[i] = tmpBuffer[i];
		}
		buffer[commPos] = '\0';
		*index = commPos;
	}
	return commands;
}
