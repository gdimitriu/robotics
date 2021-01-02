/*
 * CFactoryToFSweepPCA9685.cpp
 *
 *  Created on: Oct 12, 2020
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

#include "CFactoryToFSweepPCA9685.h"
#include <CToFSweepPCA9685.h>
#include <CLogger.h>
#include <CLoggerStdout.h>
#include <CLoggerBleHC5.h>
#include <string.h>
#include <CFactoryLogger.h>

CFactoryToFSweepPCA9685::CFactoryToFSweepPCA9685(
		CSettingLoading *settingsLoader, Adafruit_PWMServoDriver *pwmDriver,
		CLogger *logger) :
		CFactorySensor(logger) {
	this->m_settingsLoader = settingsLoader;
	this->m_pwmDriver = pwmDriver;
}

CFactoryToFSweepPCA9685::~CFactoryToFSweepPCA9685() {

}

CGenericSensor* CFactoryToFSweepPCA9685::createSensor() {
	int iChan;
	int iAddr;
	int servoPin;
	int servoCenter;
	int servoLeft;
	int servoRight;
	int position;
	int isCollision;
	int maxLeft;
	int maxRight;
	int relativePosition;
	CFactoryLogger *loggerFactory = new CFactoryLogger(m_settingsLoader);
	CLogger *logger = loggerFactory->createLogger(m_logger);
	delete loggerFactory;
	sscanf(m_settingsLoader->getLine(), "%d %d %d %d %d %d %d %d %d %d %d", &iChan, &iAddr,
			&servoPin, &servoCenter, &servoLeft, &servoRight, &position, &isCollision, &maxLeft, &maxRight, &relativePosition);
	return new CToFSweepPCA9685(iChan, iAddr, m_pwmDriver, servoPin,
			servoCenter, servoLeft, servoRight, position, isCollision, logger, maxLeft, maxRight, relativePosition);
}
