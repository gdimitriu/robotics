/*
 * CEngineSettingLoading.cpp
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

#include "CSettingLoading.h"

#include <stdlib.h>
#include <string.h>

CSettingLoading::CSettingLoading(char *fileName, CLogger *logger) {
	m_pFile = new std::ifstream(fileName);
	this->m_buffSize = 256;
	this->m_buffer = (char *)calloc(m_buffSize, sizeof(char));
	m_logger = logger;
	m_isOpenHere = true;
}

CSettingLoading::CSettingLoading(std::ifstream *pFile, CLogger *logger) {
	m_pFile = pFile;
	this->m_buffSize = 256;
	this->m_buffer = (char *)calloc(m_buffSize, sizeof(char));
	m_logger = logger;
	m_isOpenHere = false;
}

CSettingLoading::~CSettingLoading() {
	if (m_isOpenHere) {
		delete (m_pFile);
	}
	free(m_buffer);
}

CLogger *CSettingLoading::getLogger() {
	return m_logger;
}

char *CSettingLoading::getLine() {
	memset(m_buffer, 0, m_buffSize*sizeof(char));
	if (m_pFile->is_open()) {
		m_pFile->getline(m_buffer, m_buffSize, '\n');
		if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Reading line=");
			message +=m_buffer;
			message +='\n';
			m_logger->debug(message);
		}
		return m_buffer;
	}
	return NULL;
}

unsigned int CSettingLoading::getBuffSize() {
	return this->m_buffSize;
}
