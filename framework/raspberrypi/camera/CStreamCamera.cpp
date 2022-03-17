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
#include <iostream>
using namespace std;
CStreamCamera::CStreamCamera(char *configLine, CLogger *settingLogger) : CCamera() {
	m_streamingPid = -1;
	m_logger = settingLogger;
	char camera[256];
	char onOff[256];
	m_commandLine = new std::vector<std::string *>();
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
	m_commandLine->push_back(new string("libcamera-vid"));
	m_commandLine->push_back(new string("-n"));
	m_commandLine->push_back(new string("-t"));
	m_commandLine->push_back(new string("0"));
	m_commandLine->push_back(new string("--inline"));
	m_commandLine->push_back(new string("--codec"));
	m_commandLine->push_back(new string(m_codec));
	m_commandLine->push_back(new string("--framerate"));
	m_commandLine->push_back(new string(to_string(m_frameRate)));
	m_commandLine->push_back(new string("--listen"));
	if(strcmp("tcp",m_connectionType.c_str()) == 0) {
		m_commandLine->push_back(new string("-o"));
		string *tmp = new string("tcp://0.0.0.0:");
		*tmp +=to_string(m_streamingPort);
		m_commandLine->push_back(tmp);
	}
}

CStreamCamera::~CStreamCamera() {
	stopStreaming();
	if (m_commandLine != NULL) {
		for(int i = 0; i < m_commandLine->size(); i++)
			delete m_commandLine->at(i);
		delete m_commandLine;
	}
}

void CStreamCamera::startStreaming() {
	pid_t pid = fork();
	if (pid  < 0 || pid > 0) {
		m_streamingPid = pid;
		sleep(1);
		return;
	}
	char **argv = (char **)calloc(m_commandLine->size() + 1,sizeof(char *));
	for(int i = 0; i < m_commandLine->size(); i++)
		argv[i] = m_commandLine->at(i)->c_str();
	argv[m_commandLine->size()] = NULL;
	if(m_logger->isDebug()) {
		string message("StreamCameraCommand:");
		for(int i = 0; i < m_commandLine->size(); i++) {
			message += argv[i];
			message += " ";
		}
		m_logger->debug(message);
	}
	if (execvp(argv[0],argv) < 0) {
		cout<<"Could not execute command"<<endl;
	}
	_exit(-1);
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
