/*
 * CDroid.cpp
 *
 *  Created on: Oct 8, 2020
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

#include "CDroid.h"
#include <CLoggerStdout.h>
#include <CLoggerBleHC5.h>
#include <stdlib.h>
#include <pigpio.h>
#include <string.h>
#include <list>

CDroid::CDroid(char *droidCfgFile, int isOnHost) {
	m_isOnHost = isOnHost;
	m_droidCfgFile = droidCfgFile;
	m_pFile = new std::ifstream(m_droidCfgFile);
	m_logger = 0;
	m_controlEngines = 0;
	m_controlSensors = 0;
	m_buffer = 0;
	m_buffSize = 255;
	m_pwmDriver = 0;
	m_isOnHost = isOnHost;
	m_maxLeftServoEncoder = m_maxRightServoEncoder = -1; // no sweep sensor in front
	m_stopDistance = 150; //default for my platform
	m_nextDistance = 0;
	m_nextRotation = 0;
	m_moveThread = 0;
	m_actualDistance = 0;
	m_nextCheckRotate = 0;
	initialize();
}

CDroid::~CDroid() {
	if (m_controlEngines != 0) {
		delete m_controlEngines;
	}
	if (m_controlSensors != 0) {
		delete m_controlSensors;
	}
	if (m_pwmDriver != 0) {
		delete m_pwmDriver;
	}
	if (m_logger != 0) {
		delete m_logger;
	}
	if (m_isOnHost == 0) {
		gpioTerminate();
	}
}

CLogger* CDroid::getLogger() {
	return m_logger;
}

void CDroid::initialize(std::ifstream *pFile) {
	m_pFile = pFile;
	pthread_attr_init(&m_moveThreadAttr);
	initialize();
}

char* CDroid::getLine() {
	memset(m_buffer, 0, m_buffSize * sizeof(char));
	if (m_pFile->is_open()) {
		m_pFile->getline(m_buffer, m_buffSize, '\n');
		if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Reading line=");
			message += m_buffer;
			message += '\n';
			m_logger->debug(message);
		}
		return m_buffer;
	}
	return NULL;
}

void CDroid::dumpInfo() {
	m_controlEngines->dumpInfo();
}

void CDroid::initialize() {
	this->m_buffSize = 256;
	this->m_buffer = (char*) calloc(m_buffSize, sizeof(char));
	//get the logger
	char *line = getLine();
	if (m_isOnHost == 0) {
		if (gpioInitialise() < 0) {
			perror("gpio init failed");
			exit(1);
		}
	}
	if (m_logger == 0) {
		if (strcmp("CLoggerStdout", line) == 0) {
			m_logger = new CLoggerStdout();
		} else if (strcmp("CLoggerBleHC5", line) == 0) {
			m_logger = new CLoggerBleHC5();
		}
		line = getLine();
	}
	if (strcmp("Adafruit_PWMServoDriver", line) == 0) {
		m_pwmDriver = new Adafruit_PWMServoDriver();
		m_pwmDriver->begin();
		m_pwmDriver->reset();
		m_pwmDriver->setOscillatorFrequency(27000000);
		m_pwmDriver->setPWMFreq(50.0);
	}
	m_pwmDriver->setPWM(7,0,350);//grabber at center hardcodded
	//get the configuration of the engines
	line = getLine();
	m_controlEngines = new CMasterControlEngines(line, m_logger, m_pwmDriver);
	//get the configuration of the sensors
	line = getLine();
	m_controlSensors = new CMasterControlSensors(line, m_logger, m_pwmDriver);
	if (!m_pFile->eof()) {
		sscanf(getLine(), "%u", &m_stopDistance);
	} else {
		m_stopDistance = 150; //default for my platform
	}
	if (!m_pFile->eof()) {
		sscanf(getLine(), "%d %d", &m_maxLeftServoEncoder,
				&m_maxRightServoEncoder);
	} else {
		m_maxLeftServoEncoder = m_maxRightServoEncoder = -1; // no sweep sensor in front
	}
	m_moveBarrier.init(m_controlEngines->getEnginesNr() + 1);
	m_controlEngines->setMoveBarrier(&m_moveBarrier);
	struct sched_param goSchedParam;
	pthread_attr_setschedpolicy(&m_moveThreadAttr, SCHED_FIFO);
	goSchedParam.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_attr_setschedparam(&m_moveThreadAttr, &goSchedParam);
}

void CDroid::setMaxEnginePower(unsigned int newPower) {
	m_controlEngines->setMaxEnginePower(newPower);
}

void CDroid::setMinEnginePower(unsigned int newPower) {
	m_controlEngines->setMinEnginePower(newPower);
}

unsigned int CDroid::getMaxEnginePower() {
	return m_controlEngines->getMaxEnginePower();
}

unsigned int CDroid::getMinEnginePower() {
	return m_controlEngines->getMinEnginePower();
}

unsigned long CDroid::getLowPowerDistance() {
	return m_controlEngines->getLowPowerDistance();
}

void CDroid::setLowPowerDistance(unsigned long distance) {
	m_controlEngines->setLowPowerDistance(distance);
}

void CDroid::setStopDistance(unsigned int distance) {
	m_stopDistance = distance;
}

unsigned int CDroid::getStopDistance() {
	return m_stopDistance;
}

void CDroid::mainCollisionCallback(void *instance, CGenericSensor *sensor) {
	((CDroid*) instance)->collisionCallback(sensor);
}

void CDroid::collisionCallback(CGenericSensor *sensor) {
	m_controlEngines->breakEngines(2);
	m_logger->debug("stop as collision\n");
	m_moveBarrier.reset();
}

void CDroid::fullStop() {
	m_controlEngines->breakEngines(2);
	m_moveBarrier.reset();
	if (m_moveThread > 0) {
		pthread_cancel(m_moveThread);
		m_moveThread = 0;
	}
	if (m_logger->isDebug() == 1) {
		unsigned long *encoders = m_controlEngines->getEncoders();
		std::string message("Ecoders:");
		for (int i = 0; i < m_controlEngines->getEnginesNr(); i++) {
			message += std::to_string(encoders[i]);
			message += " ";
		}
		message += "\n";
		m_logger->debug(message);
		free(encoders);
	}
	m_controlSensors->unregisterCollisionCallback(0);
	m_controlSensors->unregisterCollisionCallback(180);
}

void CDroid::forwardMove(float newDistance, unsigned int newPower) {
	float distance = newDistance * 10;
	int forward = m_controlSensors->lookCenter(1);
	float actualMove = distance;
	int actualPower = newPower;
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Move forward with request in mm ");
		message += std::to_string(distance);
		message += " and in front is available ";
		message += std::to_string(forward);
		message += "\n";
		m_logger->debug(message);
	}
	if (distance >= 0 && forward < distance) {
		if (forward >= m_stopDistance) {
			actualPower = 2000;
			actualMove = forward;
		}
	} else {
		actualPower = 4000;
		actualMove = distance;
	}
	if (m_stopDistance > 0) {
		m_controlSensors->registerCollisionCallback(m_stopDistance, 0,
				CDroid::mainCollisionCallback, this);
	}
	m_controlEngines->setEnginePower(actualPower);
	m_controlEngines->moveDistance(actualMove);
}

void CDroid::backwardMove(float newDistance, unsigned int newPower) {
	m_controlSensors->registerCollisionCallback(m_stopDistance, 180,
			CDroid::mainCollisionCallback, this);
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Move backward with request in mm ");
		message += std::to_string(-newDistance * 10);
		message += "\n";
		m_logger->debug(message);
	}
	m_controlEngines->setEnginePower(newPower);
	m_controlEngines->moveDistance(newDistance * 10);
}

void CDroid::rotate(int nrEncoder, int check) {
	if (check == 0) {
		m_controlEngines->setEnginePower(4000);
		m_controlEngines->rotate(nrEncoder);
		m_moveBarrier.sync();
		return;
	}
	float distance = 0.0;
	while(distance <= m_nextDistance) {
		m_controlEngines->setEnginePower(4000);
		m_controlEngines->rotate(nrEncoder);
		m_moveBarrier.sync();
		distance = m_controlSensors->lookCenter(1);
	}
}

void CDroid::move(float distance) {
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Move with request in mm ");
		message += std::to_string(distance * 10);
		message += "\n";
		m_logger->debug(message);
	}
	if (distance > 0)
		forwardMove(distance, 400);
	else {
		backwardMove(distance, 4000);
	}
	m_moveBarrier.sync();
	if (distance > 0) {
		m_controlSensors->unregisterCollisionCallback(0);
	} else {
		m_controlSensors->unregisterCollisionCallback(180);
	}
	m_actualDistance = m_controlEngines->getActualDistance();
}

void* CDroid_InternalMoveThread(void *droidInstance) {
	((CDroid*) droidInstance)->internalMove();
	((CDroid*) droidInstance)->m_moveThread = 0;
	return 0;
}

void CDroid::move(float distance, int rotation, int check) {
	if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Received command move with distance ");
			message += std::to_string(distance);
			message += " cm and rotation ";
			message += std::to_string(rotation);
			message += " with check distance ";
			message += std::to_string(check);
			message += "\n";
		m_logger->debug(message);
	}
	if (m_moveThread == 0) {
		m_nextDistance = distance;
		m_nextRotation = rotation;
		m_nextCheckRotate = check;
		pthread_create(&m_moveThread, &m_moveThreadAttr,
				CDroid_InternalMoveThread, this);
	} else {
		m_logger->error("Droid already in move\n");
	}
}

void CDroid::internalMove() {

	if (m_nextDistance == 0) { //just rotate
		if (m_nextRotation == -1) { //left
			rotate(m_maxLeftServoEncoder, m_nextCheckRotate);
		} else if (m_nextRotation == 1) { //right
			rotate(m_maxRightServoEncoder, m_nextCheckRotate);
		} else {
			rotate(m_nextRotation, m_nextCheckRotate);
		}
		return;
	}
	if (m_nextRotation < 0) { //left
		internalMoveLeft();
	} else if (m_nextRotation == 0) { //center
		move(m_nextDistance);
	} else if (m_nextRotation > 0) { //right
		internalMoveRight();
	}
}

void CDroid::internalMoveLeft() {
	int value = 4000; //max distance
	/*
	 * rotation where we could check directly the space
	 * if the m_nextCheckRotate is on, the rotate could check
	*/
	if ((m_nextRotation == -1 || m_nextRotation == m_maxLeftServoEncoder) && m_nextCheckRotate == 0) {
		value = m_controlSensors->lookLeft(1);
		if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Look left distance in mm ");
			message += std::to_string(value);
			message += " and request distance is ";
			message += std::to_string(m_nextDistance * 10);
			message += "\n";
			m_logger->debug(message);
		}
		if (value < m_stopDistance && m_nextDistance * 10 < m_stopDistance) {
			return;
		}
	}
 	if (m_nextRotation == -1) {
		rotate(m_maxLeftServoEncoder, m_nextCheckRotate);
	} else {
		rotate(m_nextRotation, m_nextCheckRotate);
	}
	if (m_nextDistance * 10 < value)
		move(m_nextDistance);
	else
		move(value);
}

void CDroid::internalMoveRight() {
	int value = 4000; //max distance
	/*
	 * rotation where we could check directly the space
	 * if the m_nextCheckRotate is on, the rotate could check
	 */
	if ((m_nextRotation == 1 || m_nextRotation == m_maxRightServoEncoder) && m_nextCheckRotate == 0) {
		value = m_controlSensors->lookRight(1);
		if (m_logger != NULL && m_logger->isDebug() == 1) {
			std::string message("Look right distance in mm ");
			message += std::to_string(value);
			message += " and request distance is ";
			message += std::to_string(m_nextDistance * 10);
			message += "\n";
			m_logger->debug(message);
		}
		if (value < m_stopDistance && m_nextDistance * 10 < m_stopDistance) {
			return;
		}
	}
	if (m_nextRotation == 1) {
		rotate(m_maxRightServoEncoder, m_nextCheckRotate);
	} else {
		rotate(m_nextRotation, m_nextCheckRotate);
	}
	if (m_nextDistance * 10 < value)
		move(m_nextDistance);
	else
		move(value);
}
