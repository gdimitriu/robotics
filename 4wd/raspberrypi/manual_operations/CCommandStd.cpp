/*
 * CBLECommandHC05.cpp
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

#include "CCommandStd.h"

#include <iostream>
#include <string>
using namespace std;
CCommandStd::CCommandStd(CCommand *move, CCommand *setting) : CCommCommands(move, setting){

}

CCommandStd::~CCommandStd() {
}

void CCommandStd::printMenu() {
	std::cout<<"Menu\n";
	m_moveCommand->printMenu();
	m_settingCommand->printMenu();
}

void CCommandStd::startReceiving() {
	string str;
	string exitCond = "exit#";
	do {
		cin>>str;
		if (str == exitCond) {
			break;
		}
		processInputData(&str);

	} while(str != exitCond);
}
