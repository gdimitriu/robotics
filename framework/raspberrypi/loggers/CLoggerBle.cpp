/*
 * CLoggerBle.cpp
 *
 *  Created on: Jan 4, 2021
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2021 Gabriel Dimitriu
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

#include "CLoggerBle.h"
#include <pigpio.h>
#include <string>
#include <string.h>
using namespace std;

CLoggerBle::CLoggerBle() {
	m_serialHandler = 0;
}

CLoggerBle::CLoggerBle(char *tty,int boudRate) {
	m_serialHandler = serOpen(tty, boudRate, 0);
}

CLoggerBle::~CLoggerBle() {
	if (m_serialHandler != 0) {
		serClose(m_serialHandler);
	}
}


void CLoggerBle::info(const char *data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data, strlen(data));
	}
}

void CLoggerBle::debug(const char *data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data, strlen(data));
	}
}

void CLoggerBle::error(const char *data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data, strlen(data));
	}
}

void CLoggerBle::info(string data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data.c_str(),data.length());
	}
}

void CLoggerBle::debug(string data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data.c_str(),data.length());
	}
}

void CLoggerBle::error(string data) {
	if (m_serialHandler != 0) {
		serWrite(m_serialHandler, (char *)data.c_str(),data.length());
	}
}
