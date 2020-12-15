/*
 * CBLECommandHC05.h
 *
 *  Created on: Oct 5, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
 *
 * This file is part of the Robotics project.

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

#ifndef MANUAL_OPERATIONS_CBLECOMMANDHC05_H_
#define MANUAL_OPERATIONS_CBLECOMMANDHC05_H_
#include <CCommCommands.h>
#include "CCommand.h"
using namespace std;

class CBLECommandHC05 : public CCommCommands {
public:
	CBLECommandHC05(CCommand *move, CCommand *setting);
	virtual ~CBLECommandHC05();
	virtual void printMenu();
	virtual void startReceiving();
private:
	int m_serialHandler;
	int m_boudrate;
	char *m_port;
};

#endif /* MANUAL_OPERATIONS_CBLECOMMANDHC05_H_ */
