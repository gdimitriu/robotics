/*
 * CEngineSettingLoading.h
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

#ifndef SETTINGS_CSETTINGLOADING_H_
#define SETTINGS_CSETTINGLOADING_H_
#include <iostream>
#include <fstream>
#include <CLogger.h>

class CSettingLoading {
public:
	CSettingLoading(char *fileName, CLogger *logger);
	CSettingLoading(std::ifstream *m_pFile, CLogger *logger);
	virtual ~CSettingLoading();
	char* getLine();
	unsigned int getBuffSize();
	CLogger *getLogger();
private:
	CLogger *m_logger;
	std::ifstream *m_pFile;
	char *m_buffer;
	unsigned int m_buffSize;
	bool m_isOpenHere;
};

#endif /* SETTINGS_CSETTINGLOADING_H_ */
