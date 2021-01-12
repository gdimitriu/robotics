/*
 * CLoggerStdout.cpp
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

#include "CLoggerStdout.h"
#include <iostream>
#include <string>
using namespace std;

CLoggerStdout::CLoggerStdout() {
	// TODO Auto-generated constructor stub

}

CLoggerStdout::~CLoggerStdout() {
	// TODO Auto-generated destructor stub
}

void CLoggerStdout::info(const char *data) {
	cout<<data;
	cout<<flush;
}

void CLoggerStdout::debug(const char *data) {
	cout<<"DEBUG:"<<data;
	cout<<flush;
}

void CLoggerStdout::error(const char *data) {
	cout<<"ERROR:"<<data;
	cout<<flush;
}

void CLoggerStdout::info(std::string data) {
	cout<<data;
	cout<<flush;
}

void CLoggerStdout::debug(std::string data) {
	cout<<"DEBUG:"<<data;
	cout<<flush;
}

void CLoggerStdout::error(std::string data) {
	cout<<"ERROR:"<<data;
	cout<<flush;
}


