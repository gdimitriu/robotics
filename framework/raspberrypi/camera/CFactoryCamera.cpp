/*
 * CFactoryCamera.cpp
 *
 *  Created on: Feb 9, 2022
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2022 Gabriel Dimitriu
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

#include "CFactoryCamera.h"
#include <string.h>
#ifdef HAS_OLD_CAMERA
#include <COldCamera.h>
#endif
#include <CStreamCamera.h>

CFactoryCamera::CFactoryCamera(std::ifstream *pfile,CLogger *logger) {
	m_logger = logger;
	m_pFile = pfile;
	m_buffSize = 256;
	m_buffer = (char*) calloc(m_buffSize, sizeof(char));
}

CFactoryCamera::~CFactoryCamera() {
	if (m_buffer != NULL)
		free(m_buffer);
}

char* CFactoryCamera::getLine() {
	memset(m_buffer, 0, m_buffSize * sizeof(char));
	if (m_pFile->is_open()) {
		m_pFile->getline(m_buffer, m_buffSize, '\n');
		if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Reading line=");
			message += m_buffer;
			message += '\n';
			m_logger->debug(message);
		}
		return m_buffer;
	}
	return NULL;
}
CCamera *CFactoryCamera::createCamera() {
	char *line;
	line = getLine();
	if (strcmp("COldCamera",line) == 0) {
#ifdef HAS_OLD_CAMERA
		line = getLine();
		return new COldCamera(line,m_logger);
#else
		return new CCamera();
#endif
	} else if (strcmp("CStreamCamera",line) == 0) {
		line = getLine();
		return new CStreamCamera(line,m_logger);
	} else {
		//consume the data
		line = getLine();
		return new CCamera();
	}
	return NULL;
}

