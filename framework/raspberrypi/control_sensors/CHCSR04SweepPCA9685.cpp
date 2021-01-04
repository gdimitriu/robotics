/*
 * CHCSR04SweepPCA9685.cpp
 *
 *  Created on: Nov 25, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
 *
 * This file part of Robotics project.

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

#include "CHCSR04SweepPCA9685.h"
#include <chrono>

CHCSR04SweepPCA9685::CHCSR04SweepPCA9685(Adafruit_PWMServoDriver *pwmDriver, int servoPin, int servoCenter,
		int servoLeft, int servoRight, int position, int isCollision,
		CLogger *logger, int maxLeft, int maxRight, int echoPort, int trigPort, int relativePosition) :
		CSweepPCA9685(pwmDriver, servoPin, servoCenter, servoLeft, servoRight, position, isCollision, logger, maxLeft, maxRight, relativePosition) {
	m_trigPort = trigPort;
	m_echoPort = echoPort;
	gpioSetMode(m_echoPort, PI_INPUT);
	gpioSetPullUpDown(m_echoPort, PI_PUD_UP);
	gpioSetMode(m_trigPort, PI_OUTPUT);
}

CHCSR04SweepPCA9685::~CHCSR04SweepPCA9685() {
}

std::string CHCSR04SweepPCA9685::getDebugInformation() {
	std::string message("CHCSR04SweepPCA9685 echoPort=");
	message += std::to_string(m_echoPort);
	message += " trigPort=";
	message += std::to_string(m_trigPort);
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

int CHCSR04SweepPCA9685::getDistance() {
	std::chrono::microseconds timeout(26190); //450 cm ...
	int currentDistance = 4500;
	int timeoutOccured = 0;
	gpioWrite(m_trigPort,0);
	usleep(2);
	gpioWrite(m_trigPort,1);
	usleep(10);
	gpioWrite(m_trigPort,0);
	while(gpioRead(m_echoPort) == 0);
	auto start = std::chrono::high_resolution_clock::now();
	while(gpioRead(m_echoPort) == 1) {
		if ((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-start)) > timeout) {
			timeoutOccured = 1;
			break;
		}
	}
	if (timeoutOccured == 0) {
		auto finish = std::chrono::high_resolution_clock::now();
		float microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count();
		currentDistance = ((microseconds/2)/29.1) * 10; //cm * 10 to be mm
	}
	return currentDistance;
}
