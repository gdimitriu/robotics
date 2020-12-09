/*
 * CCommCommands.h
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

#ifndef MANUAL_OPERATIONS_CCOMMCOMMANDS_H_
#define MANUAL_OPERATIONS_CCOMMCOMMANDS_H_
#include "CCommand.h"
#include <set>
#include <string>
#include <CDroid.h>

class CCommCommands {
public:
	CCommCommands(CCommand *move, CCommand *setting);
	virtual ~CCommCommands();
	virtual void startReceiving() = 0;
	virtual void printMenu() = 0;
	virtual void setDroid(CDroid *droid);
protected:
	void processInputData(string *input);
	set<char> *m_moveOperations;
	set<char> *m_settingOperations;
	CCommand *m_moveCommand;
	CCommand *m_settingCommand;
	CLogger* m_logger;
};

#endif /* MANUAL_OPERATIONS_CCOMMCOMMANDS_H_ */
