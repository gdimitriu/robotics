/*
 * CLoggerFile.cpp
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

#include "CLoggerFile.h"
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

CLoggerFile::CLoggerFile(char *fileName, char append) {
	m_fileName = (char *)calloc(strlen(fileName) + 1,sizeof(char));
	strncpy(m_fileName,fileName, strlen(fileName) +1);
	if (append != 'a') {
		m_pFile = new std::ofstream(m_fileName,std::ios::out | std::ios::trunc);
	} else {
		m_pFile = new std::ofstream(m_fileName,std::ios::out | std::ios::app | std::ios::ate);
	}
}

CLoggerFile::~CLoggerFile() {
	if (m_pFile->is_open())
		m_pFile->close();
	delete m_pFile;
	free(m_fileName);
}

void CLoggerFile::info(const char *data) {
	(*m_pFile)<<data;
	m_pFile->flush();
}

void CLoggerFile::debug(const char *data) {
	(*m_pFile)<<"DEBUG:"<<data;
	m_pFile->flush();
}

void CLoggerFile::error(const char *data) {
	(*m_pFile)<<"ERROR:"<<data;
	m_pFile->flush();
}

void CLoggerFile::info(std::string data) {
	(*m_pFile)<<data;
	m_pFile->flush();
}

void CLoggerFile::debug(std::string data) {
	(*m_pFile)<<"DEBUG:"<<data;
	m_pFile->flush();
}

void CLoggerFile::error(std::string data) {
	(*m_pFile)<<"ERROR:"<<data;
	m_pFile->flush();
}
