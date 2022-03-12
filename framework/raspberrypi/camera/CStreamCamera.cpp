/*
 * CStreamCamera.cpp
 *
 *  Created on: Mar 10, 2022
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

#include "CStreamCamera.h"
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sstream>
using namespace std;
CStreamCamera::CStreamCamera(char *configLine, CLogger *settingLogger) : CCamera() {
	m_streamingPid = -1;
	m_logger = settingLogger;
	char camera[256];
	char onOff[256];
	memset(camera, 0, 256 * sizeof(char));
	memset(onOff, 0, 256 * sizeof(char));
	m_connectionType = string("tcp");
	m_streamingPort = 8120;
	m_codec = string("mjpeg");
	m_frameRate = 2;
	if (strncmp("camera",configLine, 6) != 0) {
		string message("Camera configuration is missing");
		settingLogger->error(message);
		return;
	}
	istringstream configStream(configLine);
	configStream >> camera >> onOff >> m_connectionType >> m_streamingPort >> m_codec >> m_frameRate;
	if (strcmp("on",onOff) == 0)
		m_isDisabled = false;
	else {
		string message("Camera is disabled");
		settingLogger->info(message);
		return;
	}
	if (m_logger->isDebug()) {
		char message[512];
		memset(message, 0, 512 * sizeof(char));
		sprintf(message,"StreamCamera: conn=%s:%d codec=%s framerate=%d\n",m_connectionType.c_str(), m_streamingPort,m_codec.c_str(),m_frameRate);
		m_logger->debug(message);
	}
}

CStreamCamera::~CStreamCamera() {
	stopStreaming();
}

void CStreamCamera::startStreaming() {

}

void CStreamCamera::stopStreaming() {
	if (m_streamingPid > 0) {
		kill(m_streamingPid, SIGKILL);
		m_streamingPid = -1;
	}
}

string* CStreamCamera::getInfo() {
	string *info = new string("StreamCamera:");
	*info += m_isDisabled ? "off," : "on,";
	*info += m_connectionType;
	*info += ",";
	*info += std::to_string(m_streamingPort);
	*info += ",";
	*info += m_codec;
	*info += ",";
	*info += std::to_string(m_frameRate);
	*info += "#";
	return info;
}
