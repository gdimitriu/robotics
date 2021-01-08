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

CManualDroid::CManualDroid(char *droidCfgFile, int isOnHost, CCommCommands *command) : CDroid(droidCfgFile, isOnHost) {
	m_command = command;
	m_command->setDroid(this);
}

CManualDroid::~CManualDroid() {
	delete m_command;
}

void CManualDroid::dumpInfo() {
	m_command->printMenu();
}

void CManualDroid::startReceiving() {
	m_command->startReceiving();
}

