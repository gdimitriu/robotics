/*
 * CCommCommands.h
 *
 *  Created on: Oct 8, 2020
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

#ifndef MANUAL_OPERATIONS_CCOMMCOMMANDS_H_
#define MANUAL_OPERATIONS_CCOMMCOMMANDS_H_
#include "CCommand.h"
#include <set>
#include <vector>
#include <string>
#include <CDroid.h>

class CCommCommands {
public:
	CCommCommands(CCommand *move, CCommand *setting);
	virtual ~CCommCommands();
	virtual void startReceiving() = 0;
	virtual void printMenu();
	virtual void setDroid(CDroid *droid);
	virtual int executeLocal(const char *operation);
	virtual void stop();
	static int isStopped();

protected:
	/*
	 * split the buffer into commands using # as delimitator
	 * return commands vector with full commands
	 * in buffer remain the last uncomplited command with the correspoinding index
	 */
	vector<string *> *splitCommandsAndClearBuffer(char *buffer,int *index,int maxSize);
	int processInputData(string *input, int reverse = 0);
	void removeCommandPrefix(char *operation);
	CCommand* getSettingCommand();
	set<char> *m_moveOperations;
	set<char> *m_settingOperations;
	set<char> m_localOperations;
	CCommand *m_moveCommand;
	CCommand *m_settingCommand;
	CLogger* m_logger;
	//list of recorded movement
	vector<string *> m_recordMovement;
	bool m_isRecording;
	string *m_menu;
	CDroid *m_droid;
	static bool m_isStopped;
	bool m_hasAck;
};

#endif /* MANUAL_OPERATIONS_CCOMMCOMMANDS_H_ */
