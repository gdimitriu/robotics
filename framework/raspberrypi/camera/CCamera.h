/*
 * CCamera.h
 *
 *  Created on: May 10, 2021
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

#ifndef CAMERA_CCAMERA_H_
#define CAMERA_CCAMERA_H_

#include <raspicam_still.h>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <CLogger.h>

class CCamera {
public:
	CCamera(char *configLine, CLogger *settingLogger);
	virtual ~CCamera();
	void captureCameraImage(std::ofstream *pFile);
	void captureHighResolutionImage(std::ofstream *pFile);
private:
	raspicam::RaspiCam_Still *m_pCamera;
	bool m_isDisabled;
	int m_width;
	int m_height;
	int m_iso;
};

#endif /* CAMERA_CCAMERA_H_ */
