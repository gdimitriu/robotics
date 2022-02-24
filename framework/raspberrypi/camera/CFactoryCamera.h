/*
 * CCameraFactory.h
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

#ifndef CAMERA_CFACTORYCAMERA_H_
#define CAMERA_CFACTORYCAMERA_H_

#include <CLogger.h>
#include <CCamera.h>
#include <iostream>
#include <fstream>

class CFactoryCamera {
public:
	CFactoryCamera(std::ifstream *pfile,CLogger *logger);
	virtual ~CFactoryCamera();
	CCamera *createCamera();
private:
	char* getLine();
	std::ifstream *m_pFile;
	CLogger *m_logger;
	char *m_buffer;
	unsigned int m_buffSize;
};

#endif /* CAMERA_CFACTORYCAMERA_H_ */
