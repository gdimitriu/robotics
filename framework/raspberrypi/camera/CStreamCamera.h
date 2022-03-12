/*
 * CStreamCamera.h
 *
 *  Created on: Mar 10, 2022
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

#ifndef CAMERA_CSTREAMCAMERA_H_
#define CAMERA_CSTREAMCAMERA_H_

#include "CCamera.h"
#include <CLogger.h>
#include <string>

class CStreamCamera: public CCamera {
public:
	CStreamCamera(char *configLine, CLogger *settingLogger);
	virtual ~CStreamCamera();
	virtual void startStreaming();
	virtual void stopStreaming();
	virtual std::string* getInfo();
private:
	CLogger *m_logger;
	pid_t m_streamingPid;
	std::string m_connectionType;
	uint m_streamingPort;
	std::string m_codec;
	uint m_frameRate;
};

#endif /* CAMERA_CSTREAMCAMERA_H_ */
