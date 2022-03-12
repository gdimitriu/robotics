/*
 * COldCamera.h
 *
 *  Created on: Feb 9, 2022
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

#ifndef CAMERA_COLDCAMERA_H_
#define CAMERA_COLDCAMERA_H_

#include "CCamera.h"
#include <raspicam_still.h>
#include <stdlib.h>
#include <unistd.h>
#include <CLogger.h>

class COldCamera: public CCamera {
public:
	COldCamera(char *configLine, CLogger *settingLogger);
	virtual ~COldCamera();
	virtual void captureCameraImage(std::ofstream *pFile);
	virtual std::string* getInfo();
private:
	raspicam::RaspiCam_Still m_Camera;
	int m_width;
	int m_height;
	int m_iso;

};

#endif /* CAMERA_COLDCAMERA_H_ */
