/*
 * CCommand.cpp
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

#include "CCommand.h"
#include <algorithm>
#include <string.h>

CCommand::CCommand() {
	m_logger = NULL;
}

CCommand::CCommand(CLogger *logger) {
	m_logger = logger;
}

CCommand::~CCommand() {
	// TODO Auto-generated destructor stub
}

int CCommand::isValidNumber(char *data) {
	int size = strlen(data);
	if (size == 0)
		return 0;
	int isNum = 0;
	if (!(data[0] =='+' || data[0] =='-' || std::isdigit(data[0])))
		return 0;
	for (int i = 0; i < size; i++) {
		if (!(std::isdigit(data[i]) || data[i] == '.'))
			return 0;
	}
	return 1;
}

int CCommand::execute(const char *operation) {
	if (operation == NULL) {
		return 0;
	}
	if (strlen(operation) < 2)
		return 0;

	char * newOperation = (char *)calloc(strlen(operation) +1, sizeof(char));
	strcpy(newOperation, operation);
	//clear the #
	newOperation[strlen(newOperation) -1] = '\0';
	int result = 0;
	if (strlen(newOperation) == 1) {
		result = executeSimpleCommand(newOperation);
	} else if(strlen(newOperation) > 1) {
		result = executeDataCommand(newOperation);
	} else {
		char message[255];
		sprintf(message,"Invalid operation = %s\n", operation);
		m_logger->error(message);
	}
	free(newOperation);
	return result;
}

void CCommand::removeCommandPrefix(char *operation) {
	int size = strlen(operation);
	for (int i = 0; i < size; i++) {
		operation[i] = operation[i+1];
	}
	operation[size+1] = '\0';
}
