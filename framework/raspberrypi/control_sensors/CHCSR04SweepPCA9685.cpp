/*
 * CHCSR04SweepPCA9685.cpp
 *
 *  Created on: Nov 25, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CHCSR04SweepPCA9685.h"
#include <chrono>

CHCSR04SweepPCA9685::CHCSR04SweepPCA9685(Adafruit_PWMServoDriver *pwmDriver, int servoPin, int servoCenter,
		int servoLeft, int servoRight, int position, int isCollision,
		CLogger *logger, int maxLeft, int maxRight, int echoPort, int trigPort) :
		CSweepPCA9685(pwmDriver, servoPin, servoCenter, servoLeft, servoRight, position, isCollision, logger, maxLeft, maxRight) {
	m_trigPort = trigPort;
	m_echoPort = echoPort;
	if (m_logger != NULL && m_logger->isDebug() == 1) {
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
		if (isCollision == 1) {
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
		message += "\n";
		m_logger->debug(message);
	}
	gpioSetMode(m_echoPort, PI_INPUT);
	gpioSetPullUpDown(m_echoPort, PI_PUD_UP);
	gpioSetMode(m_trigPort, PI_OUTPUT);
	pthread_mutex_init(&m_mutex, 0);
}

CHCSR04SweepPCA9685::~CHCSR04SweepPCA9685() {
	pthread_mutex_destroy(&m_mutex);
}

int CHCSR04SweepPCA9685::getDistance() {
	std::chrono::microseconds timeout(26190); //450 cm ...
	int currentDistance = 4500;
	int timeoutOccured = 0;
	pthread_mutex_lock(&m_mutex);
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
		currentDistance = std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count() * 10;
	}
	pthread_mutex_unlock(&m_mutex);
	return currentDistance;
}
