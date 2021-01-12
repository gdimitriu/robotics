/*
 * CMoveCommand.h
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

#ifndef MANUAL_OPERATIONS_CMOVECOMMAND_H_
#define MANUAL_OPERATIONS_CMOVECOMMAND_H_
#include "CCommand.h"
#include <CDroid.h>

class CMoveCommand : public CCommand {
public:
	CMoveCommand();
	CMoveCommand(CLogger *logger);
	virtual ~CMoveCommand();
	/*
	 * get the operation prefix
	 * s# full stop
	 * fxxx# move forward xxx cm
	 * bxxx# move backward xxx cm
	 * l# rotate left 45 degree and restore safe distance
	 * lxxx# rotate left in step of 45 degree where is distance and move xxx cm
	 * r# rotate right 45 degree and restore safe distance
	 * rxxx# rotate right in step of 45 degree where is distance and move xxx cm
	 */
	virtual set<char> *getOperationsPrefix();
	/*
	 * print the menu on logger
	 */
	virtual void printMenu();
	virtual void setDroid(CDroid *droid);
protected:
	virtual int executeSimpleCommand(char *operation);
	virtual int executeDataCommand(char *operation);
	CDroid *m_droid;
private:
	void init();
	set<char> *m_operations;
	string *m_menu;
};

#endif /* MANUAL_OPERATIONS_CMOVECOMMAND_H_ */
