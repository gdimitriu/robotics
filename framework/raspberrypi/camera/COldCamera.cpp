/*
 * COldCamera.cpp
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

#include "COldCamera.h"
#include <stdio.h>
#include <string.h>

COldCamera::COldCamera(char * configLine, CLogger *settingLogger) : CCamera(){
	char camera[255];
	char onOff[255];
	if (strncmp("camera",configLine, 6) != 0) {
		std::string message("Camera configuration is missing");
		settingLogger->error(message);
		return;
	}
	sscanf(configLine, "%s %s %d %d %d", &camera, &onOff, &m_width, &m_height, &m_iso);
	if (strcmp("on",onOff) == 0)
		m_isDisabled = false;
	else {
		return;
	}
	m_Camera.setWidth ( m_width);
	m_Camera.setHeight ( m_height);
	m_Camera.setISO(m_iso);
	m_Camera.setEncoding ( raspicam::RASPICAM_ENCODING_JPEG );
	if ( !m_Camera.open()) {
		std::string message("Camera is missing on droid");
		settingLogger->error(message);
		m_isDisabled = true;
	}
}

COldCamera::~COldCamera() {
}

void COldCamera::captureCameraImage(std::ofstream *pFile) {
	if (m_isDisabled)
		return;
	unsigned int length = m_Camera.getImageBufferSize(); // Header + Image Data + Padding
	unsigned char * data = new unsigned char[length];
	if ( !m_Camera.grab_retrieve(data, length) ) {
		delete data;
		return;
	}
	pFile->write ( ( char* ) data,   length );
	delete data;
}

std::string* COldCamera::getInfo() {
	std::string *info = new std::string("OldCamera:");
	*info += m_isDisabled ? "off," : "on,";
	*info += std::to_string(m_height);
	*info += ",";
	*info += std::to_string(m_width);
	*info += ",";
	*info += std::to_string(m_iso);
	*info += "#";
	return info;
}

