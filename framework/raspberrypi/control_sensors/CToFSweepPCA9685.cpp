/*
 * CToFSweepPCA9685.cpp
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

#include "CToFSweepPCA9685.h"
#include <tof.h>
#include <Adafruit_PWMServoDriver.h>
#include <string>

CToFSweepPCA9685::CToFSweepPCA9685(int iChan, int iAddr,
		Adafruit_PWMServoDriver *pwmDriver, int servoPin, int servoCenter,
		int servoLeft, int servoRight, int position, int isCollision,
		CLogger *logger, int maxLeft, int maxRight, int relativePosition) :
		CSweepPCA9685(pwmDriver, servoPin, servoCenter, servoLeft, servoRight, position, isCollision, logger, maxLeft, maxRight, relativePosition) {
	m_i2cChannel = iChan;
	m_i2cAddress = iAddr;
	tofInit(iChan, iAddr, 1);
}

CToFSweepPCA9685::~CToFSweepPCA9685() {

}

std::string CToFSweepPCA9685::getDebugInformation() {
	std::string message("CToFSweepPCA9685 channel=");
	message += std::to_string(m_i2cChannel);
	message += " Address=";
	message += std::to_string(m_i2cAddress);
	message += " servoPin=";
	message += std::to_string(m_servoPin);
	message += " servoCenter=";
	message += std::to_string(m_servoCenter);
	message += " servoLeft=";
	message += std::to_string(m_servoLeft);
	message += " servoRight=";
	message += std::to_string(m_servoRight);
	if (m_isCollision == 1) {
		message += " is collision avoidance sensor ";
	}
	switch (m_position) {
	case 0:
		message += " in front position";
		break;
	case 90:
		message += " in right position";
		break;
	case 180:
		message += " in back position";
		break;
	case 270:
		message += " in left position";
		break;
	}
	message += " servo max left=";
	message += std::to_string(m_servoMaxLeft);
	message += " servo max right=";
	message += std::to_string(m_servoMaxRight);
	switch (m_relativePosition) {
	case -1:
		message +=" in left relative";
		break;
	case 0:
		message +=" in center relative";
		break;
	case 1:
		message +=" in right relative";
		break;
	}
	message += "\n";
	return message;
}
int CToFSweepPCA9685::getDistance() {
	return tofReadDistance();
}
