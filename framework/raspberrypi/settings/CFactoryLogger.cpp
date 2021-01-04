/*
 * CLoggerFactory.cpp
 *
 *  Created on: Jan 2, 2021
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

#include <CLoggerBleHC5.h>
#include <CLoggerStdout.h>
#include <CLoggerBle.h>
#include <CLoggerFile.h>
#include <string.h>
#include "CFactoryLogger.h"

CFactoryLogger::CFactoryLogger(CSettingLoading *settingsLoader) {
	m_Loader = settingsLoader;
}

CFactoryLogger::~CFactoryLogger() {

}

CLogger *CFactoryLogger::createLogger(CLogger *settingsLogger) {
	CLogger *logger;
	int debug;
	char *type = m_Loader->getLine();
	if (strcmp("CLoggerStdout", type) == 0) {
		logger = new CLoggerStdout();
		type = m_Loader->getLine();
		sscanf(type,"%d",&debug);
		logger->setType(debug);
	} else if (strcmp("CLoggerBleHC5", type) == 0) {
		logger = new CLoggerBleHC5();
		type = m_Loader->getLine();
		sscanf(type,"%d",&debug);
		logger->setType(debug);
	} else if (strcmp("CLoggerBle", type) == 0) {
		char tty[255];
		int boudRate;
		type = m_Loader->getLine();
		sscanf(type,"%d %s %d",&debug,&tty,&boudRate);
		logger = new CLoggerBle(tty,boudRate);
		logger->setType(debug);
	} else if (strcmp("CLoggerFile", type) == 0) {
		char fileName[255];
		type = m_Loader->getLine();
		char openType;
		sscanf(type,"%d %s %c",&debug,&fileName,&openType);
		logger = new CLoggerFile(fileName,openType);
		logger->setType(debug);
	} else {
		logger = new CLoggerStdout();
	}
	return logger;
}
