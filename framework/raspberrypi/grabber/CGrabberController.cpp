/*
 * CGrabberController.cpp
 *
 *  Created on: Dec 22, 2020
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

#include "CGrabberController.h"
#include <CSettingLoading.h>
#include <CLogger.h>
#include <CLoggerStdout.h>
#include <CLoggerBleHC5.h>
#include <string.h>
#include <CMasterControlSensors.h>
#include <CFactoryLogger.h>
#include <string>

CGrabberController::CGrabberController(char *configFile,
		CLogger *settingLogger, Adafruit_PWMServoDriver *pwmDriver, CMasterControlSensors *sensors) {
	CSettingLoading *settings = new CSettingLoading(configFile, settingLogger);
	m_controlSensors = sensors;
	CFactoryLogger *loggerFactory = new CFactoryLogger(settings);
	CLogger *logger = loggerFactory->createLogger(settingLogger);
	int pin;
	int openGrabber;
	int closeGrabber;
	int position;
	sscanf(settings->getLine(), "%d %d %d %d",&pin,&closeGrabber,&openGrabber,&position);
	m_grabber = new CGrabber(pwmDriver, position, pin, closeGrabber, openGrabber,logger);
	if (settingLogger != NULL && settingLogger->isInfo()) {
		settingLogger->info(m_grabber->getDebugInformation());
	}
	delete settings;
	delete loggerFactory;
}

CGrabberController::~CGrabberController() {
	if (m_grabber != NULL) {
		delete m_grabber;
	}
}

void CGrabberController::openClaw(int position) {
	if (m_grabber->getPosition() == position) {
		m_grabber->openClaw();
	}
}
void CGrabberController::closeClaw(int position) {
	if (m_grabber->getPosition() == position) {
		m_grabber->closeClaw();
	}
}
