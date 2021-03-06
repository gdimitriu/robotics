/*
 * CCommand.h
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

#ifndef MANUAL_OPERATIONS_CCOMMAND_H_
#define MANUAL_OPERATIONS_CCOMMAND_H_
#include <set>
#include <string>
#include <CLogger.h>
#include <CDroid.h>

using namespace std;
class CCommand {
public:
	CCommand();
	CCommand(CLogger *logger);
	virtual ~CCommand();
	/*
	 * return the prefix of the supported operations
	 */
	virtual set<char>* getOperationsPrefix() = 0;
	/*
	 * print the menu on logger
	 */
	virtual void printMenu() = 0;
	/*
	 * execute the operation
	 * operation char array as C string
	 * return 1 if it executed with succes 0 otherwise
	 */
	virtual int execute(const char *operation);
	/*
	 * validate if the data is a valid number
	 * data pointer to the number representation as C string
	 * return 1 if is a number or 0 otherwise
	 */
	int isValidNumber(char *data);
	virtual void setDroid(CDroid *droid) = 0;
protected:
	/*
	 * execute the operation without data
	 * operation char array as C string
	 * return 1 if it executed with succes 0 otherwise
	 */
	virtual int executeSimpleCommand(char *operation) = 0;
	/*
	 * execute the operation with data
	 * operation char array as C string
	 * return 1 if it executed with succes 0 otherwise
	 */
	virtual int executeDataCommand(char *operation) = 0;

	/*
	 * remove the prefix from the operation
	 * now is just a character
	 * this modify the argument
	 */
	void removeCommandPrefix(char *operation);
	CLogger * m_logger;
};

#endif /* MANUAL_OPERATIONS_CCOMMAND_H_ */
