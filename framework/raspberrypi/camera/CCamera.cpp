/*
 * CCamera.cpp
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

#include "CCamera.h"
#include <stdio.h>
#include <string.h>

CCamera::CCamera(char * configLine, CLogger *settingLogger) {
	char camera[255];
	char onOff[255];
	m_isDisabled = true;
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
	m_pCamera = new raspicam::RaspiCam_Still();
	m_pCamera->setWidth ( m_width);
	m_pCamera->setHeight ( m_height);
	m_pCamera->setISO(m_iso);
	m_pCamera->setEncoding ( raspicam::RASPICAM_ENCODING_JPEG );
	if ( !m_pCamera->open()) {
		std::string message("Camera is missing on droid");
		settingLogger->error(message);
		m_isDisabled = true;
	}
}

CCamera::~CCamera() {
	delete m_pCamera;
}

void CCamera::captureCameraImage(std::ofstream *pFile) {
	if (m_isDisabled)
		return;
	if (m_pCamera->getWidth() != m_width || m_pCamera->getHeight() != m_height) {
		delete m_pCamera;
		m_pCamera = new raspicam::RaspiCam_Still();
		m_pCamera->setWidth ( m_width);
		m_pCamera->setHeight ( m_height);
		m_pCamera->setISO(m_iso);
		m_pCamera->setEncoding ( raspicam::RASPICAM_ENCODING_JPEG );
		m_pCamera->open();
	}
	unsigned int length = m_pCamera->getImageBufferSize(); // Header + Image Data + Padding
	unsigned char * data = new unsigned char[length];
	if ( !m_pCamera->grab_retrieve(data, length) ) {
		delete data;
		return;
	}
	pFile->write ( ( char* ) data,   length );
	delete data;
}

void CCamera::captureHighResolutionImage(std::ofstream *pFile) {
	if (m_isDisabled)
		return;
	if (m_pCamera->getWidth() != 2592 || m_pCamera->getHeight() != 1944) {
		delete m_pCamera;
		m_pCamera = new raspicam::RaspiCam_Still();
		m_pCamera->setWidth ( 2592);
		m_pCamera->setHeight ( 1944);
		m_pCamera->setISO(m_iso);
		m_pCamera->setEncoding ( raspicam::RASPICAM_ENCODING_JPEG );
		m_pCamera->open();
	}
	unsigned int length = m_pCamera->getImageBufferSize(); // Header + Image Data + Padding
	unsigned char * data = new unsigned char[length];
	if ( !m_pCamera->grab_retrieve(data, length) ) {
		delete data;
		return;
	}
	pFile->write ( ( char* ) data,   length );
	delete data;
}
