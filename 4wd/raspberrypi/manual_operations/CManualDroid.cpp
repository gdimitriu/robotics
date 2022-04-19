/*
 * CManualDroid.cpp
 *
 *  Created on: Nov 29, 2020
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

#include "CManualDroid.h"
using namespace std;

CManualDroid::CManualDroid(char *droidCfgFile, int isOnHost,vector<CCommCommands *> *commands ) : CDroid(droidCfgFile, isOnHost) {
	m_commands = commands;
	for(vector<CCommCommands *>::iterator it = m_commands->begin(); it != m_commands->end(); ++it) {
		(*it)->setDroid(this);
	}
	m_threads = new pthread_t[m_commands->size()];
}
CManualDroid::~CManualDroid() {
	stopReceiving();
	for(vector<CCommCommands *>::iterator it = m_commands->begin(); it != m_commands->end(); ++it) {
		delete (*it);
	}
	delete m_commands;
	pthread_attr_destroy(&m_attr);
	delete[] m_threads;
}

void CManualDroid::dumpInfo() {
	(*m_commands)[0]->printMenu();
}

struct CommandCallStructure {
	CManualDroid *droidCommandCenter;
	CCommCommands *command;
};

void *manualDroidCommandExecute(void *command) {
	CommandCallStructure *cmd = ((CommandCallStructure *)command);
	cmd->command->startReceiving();
	cmd->droidCommandCenter->stopReceiving();
	delete cmd;
	return NULL;
}

void CManualDroid::stopReceiving() {
	(*(m_commands->begin()))->stop();
	for (int i = 0 ;i < m_commands->size(); i++)
		pthread_cancel(m_threads[i]);
}
void CManualDroid::startReceiving() {
	pthread_attr_init(&m_attr);
	for(int i = 0; i < m_commands->size(); i++) {
		CommandCallStructure *arg = new CommandCallStructure();
		arg->droidCommandCenter = this;
		arg->command = m_commands->at(i);
		pthread_create(&m_threads[i],&m_attr, manualDroidCommandExecute,(void *)arg);
	}
	for (int i = 0 ;i < m_commands->size(); i++)
		pthread_join(m_threads[i], NULL);
}
