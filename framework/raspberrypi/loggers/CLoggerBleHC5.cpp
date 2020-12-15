/*
 * CLoggerBleHC5.cpp
 *
 *  Created on: Oct 7, 2020
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

#include "CLoggerBleHC5.h"
#include <pigpio.h>
#include <string>
#include <string.h>
using namespace std;

CLoggerBleHC5::CLoggerBleHC5() {
	m_serialHandler = serOpen("/dev/ttyAMA0", 38400, 0);
}

CLoggerBleHC5::~CLoggerBleHC5() {
	serClose(m_serialHandler);
}

void CLoggerBleHC5::info(const char *data) {
	serWrite(m_serialHandler, (char *)data, strlen(data));
}

void CLoggerBleHC5::debug(const char *data) {
	serWrite(m_serialHandler, (char *)data, strlen(data));
}

void CLoggerBleHC5::error(const char *data) {
	serWrite(m_serialHandler, (char *)data, strlen(data));
}

void CLoggerBleHC5::info(string data) {
	serWrite(m_serialHandler, (char *)data.c_str(),data.length());
}

void CLoggerBleHC5::debug(string data) {
	serWrite(m_serialHandler, (char *)data.c_str(),data.length());
}

void CLoggerBleHC5::error(string data) {
	serWrite(m_serialHandler, (char *)data.c_str(),data.length());
}
int CLoggerBleHC5::isDebug() {
	return 1;
}
