/*
 * CEnginePCA9685EncMX1508.cpp
 * Engine for raspberry Pi Zero with PCA9865 and MX1508 with encoders
 *  Created on: Oct 2, 2020
 *      Author: Gabriel Dimitriu
 *
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

#include "CEnginePCA9685EncMX1508.h"

#include <iostream>
#include <string>
#include <sched.h>

unsigned int CEnginePCA9685EncMX1508::maxEnginePower = 4000;
unsigned int CEnginePCA9685EncMX1508::minEnginePower = 2000;
unsigned long CEnginePCA9685EncMX1508::lowPowerDistance = 300;

CEnginePCA9685EncMX1508::~CEnginePCA9685EncMX1508() {
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, 0, this);
	breakEngine();
	if (m_goTh > 0)
		pthread_cancel(m_goTh);
	pthread_attr_destroy(&m_goThAttr);
	pthread_cond_broadcast(&m_isrCond);
//	pthread_cond_destroy(&m_isrCond);
	pthread_mutex_destroy(&m_isrMutex);
}

CEnginePCA9685EncMX1508::CEnginePCA9685EncMX1508(unsigned int engineNr, unsigned int port, int edge, unsigned int engPin1, unsigned int engPin2) :
	CEngineWithEncoder(){
	this->m_encPort = port;
	this->m_encEdge = edge;
	this->m_enginePin1 = engPin1;
	this->m_enginePin2 = engPin2;
	this->m_pwmDriver = 0;
	this->m_encoderCount = 0;
	this->m_enginePower = 4000;
	this->m_ppi = 0.0;
	this->m_goTh = 0;
	this->m_targetDistance = 0.0f;
	this->m_currentDistance = 0.0f;
	this->m_targetEncoder = 0;
	this->m_engineNr = engineNr;
	this->m_requestedDistance = 0.0f;
	this->m_actualPower = maxEnginePower;
	pthread_mutex_init(&m_isrMutex, 0);
	pthread_cond_init(&m_isrCond, 0);
	pthread_attr_init(&m_goThAttr);
	pthread_attr_setschedpolicy(&m_goThAttr, SCHED_FIFO);
	m_goSchedParam.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_attr_setschedparam(&m_goThAttr, &m_goSchedParam);
	m_stopped = 1;
	m_requestedDirection = 1;
}

void CEnginePCA9685EncMX1508::setMaxEnginePower(unsigned int newPower) {
	this->maxEnginePower = newPower;
}

void CEnginePCA9685EncMX1508::setMinEnginePower(unsigned int newPower) {
	this->minEnginePower = newPower;
}

unsigned int CEnginePCA9685EncMX1508::getMaxEnginePower() {
	return maxEnginePower;
}

unsigned int CEnginePCA9685EncMX1508::getMinEnginePower() {
	return minEnginePower;
}

unsigned long CEnginePCA9685EncMX1508::getLowPowerDistance() {
	return lowPowerDistance;
}

void CEnginePCA9685EncMX1508::setLowPowerDistance(unsigned long distance) {
	lowPowerDistance = distance;
}

void CEnginePCA9685EncMX1508::setPWMDriver(Adafruit_PWMServoDriver *driver) {
	this->m_pwmDriver = driver;
}

void *CEnginePCA9685EncMX1508::getPWMDriver() {
	return m_pwmDriver;
}
unsigned int CEnginePCA9685EncMX1508::getEngineNr() {
	return m_engineNr;
}

void CEnginePCA9685EncMX1508::startEncoder() {
	m_encoderCount = 0;
	m_stopped = 0;
	gpioSetMode(m_encPort, PI_INPUT);
	gpioSetPullUpDown(m_encPort, PI_PUD_UP);
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, CEnginePCA9685EncMX1508::ISRMain, this);
}

void CEnginePCA9685EncMX1508::stopEncoder() {
	pthread_mutex_lock(&m_isrMutex);
	pthread_cond_signal(&m_isrCond);
	pthread_mutex_unlock(&m_isrMutex);
	if (m_stopped == 0) {
		m_stopped  = 1;
	}
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, 0, this);
	sched_yield();
}

void CEnginePCA9685EncMX1508::clearEncoder() {
	m_encoderCount = 0;
}

unsigned long CEnginePCA9685EncMX1508::getEncoder() {
	return m_encoderCount;
}

void CEnginePCA9685EncMX1508::ISRMain(int gpio, int level, uint32_t tick,
		void *holder) {
	((CEnginePCA9685EncMX1508*) holder)->encISR(gpio, level, tick);
}

void CEnginePCA9685EncMX1508::encISR(int gpio, int level, uint32_t tick) {
	m_encoderCount++;
	pthread_mutex_lock(&m_isrMutex);
	pthread_cond_signal(&m_isrCond);
	pthread_mutex_unlock(&m_isrMutex);
	sched_yield();
}

void CEnginePCA9685EncMX1508::setEnginePower(unsigned int power) {
	if (power > maxEnginePower) {
		m_enginePower = maxEnginePower;
	} else if (power < minEnginePower) {
		m_enginePower = minEnginePower;
	} else {
		m_enginePower = power;
	}
}

int CEnginePCA9685EncMX1508::isStopped() {
	return m_stopped;
}

unsigned int CEnginePCA9685EncMX1508::getEnginePower() {
	return m_enginePower;
}

void CEnginePCA9685EncMX1508::breakEngine(int type) {
	m_pwmDriver->setPWM(m_enginePin1, 0, 4095);
	m_pwmDriver->setPWM(m_enginePin2, 0, 4095);
	if (type != 0) {
		m_stopped  = 2;
	}
	stopEncoder();
}

void CEnginePCA9685EncMX1508::coastEngine() {
	m_pwmDriver->setPWM(m_enginePin1, 0, 0);
	m_pwmDriver->setPWM(m_enginePin2, 0, 0);
	stopEncoder();
}

void CEnginePCA9685EncMX1508::setPPI(float ppi) {
	m_ppi = ppi;
}

std::string CEnginePCA9685EncMX1508::getDebugInformation() {
	std::string message("EngineNR=");
	message +=std::to_string(m_engineNr);
	message +=" with encport=";
	message +=std::to_string(m_encPort);
	message +=" encEdge=";
	message +=std::to_string(m_encEdge);
	message +=" pin1=";
	message +=std::to_string(m_enginePin1);
	message +=" pin2=";
	message +=std::to_string(m_enginePin2);
	message +=" ppi=";
	message +=std::to_string(m_ppi);
	message +="\n";
	return message;
}

void CEnginePCA9685EncMX1508::dumpInfo(CLogger *logger) {
	if (logger != NULL) {
		std::string message("EngineNR=");
		message +=std::to_string(m_engineNr);
		message +=" with encport=";
		message +=std::to_string(m_encPort);
		message +=" encEdge=";
		message +=std::to_string(m_encEdge);
		message +=" pin1=";
		message +=std::to_string(m_enginePin1);
		message +=" pin2=";
		message +=std::to_string(m_enginePin2);
		message +=" ppi=";
		message +=std::to_string(m_ppi);
		message +="\n";
		logger->info(message);
	}
}

void CEnginePCA9685EncMX1508::startMoving() {
	if (m_requestedDistance > 0) {
		m_pwmDriver->setPWM(m_enginePin2, 0, 0);
	} else if (m_requestedDistance < 0) {
		m_pwmDriver->setPWM(m_enginePin1, 0, 0);
	}
	pthread_barrier_wait(m_startBarrier);
	if (m_requestedDistance > 0) {
		m_pwmDriver->setPWM(m_enginePin1, 0, m_actualPower);
	} else if (m_requestedDistance < 0) {
		m_pwmDriver->setPWM(m_enginePin2, 0, m_actualPower);
	} else {
		m_pwmDriver->setPWM(m_enginePin1, 0, 0);
		m_pwmDriver->setPWM(m_enginePin2, 0, 0);
	}
}

void CEnginePCA9685EncMX1508::startMovingWOEncoder() {
	if (m_requestedDirection > 0) {
		m_pwmDriver->setPWM(m_enginePin2, 0, 0);
	} else if (m_requestedDirection < 0) {
		m_pwmDriver->setPWM(m_enginePin1, 0, 0);
	}
	pthread_barrier_wait(m_startBarrier);
	if (m_requestedDirection > 0) {
		m_pwmDriver->setPWM(m_enginePin1, 0, m_actualPower);
	} else if (m_requestedDirection < 0) {
		m_pwmDriver->setPWM(m_enginePin2, 0, m_actualPower);
	} else {
		m_pwmDriver->setPWM(m_enginePin1, 0, 0);
		m_pwmDriver->setPWM(m_enginePin2, 0, 0);
	}
}

void CEnginePCA9685EncMX1508_moveCleanup(void *arg) {
	pthread_mutex_t *mutex = (pthread_mutex_t *)arg;
	pthread_mutex_unlock(mutex);
}

void* CEnginePCA9685EncMX1508_moveWOEncoder(void *engine) {
	CEnginePCA9685EncMX1508 *currentEngine = (CEnginePCA9685EncMX1508*) engine;
	currentEngine->startMovingWOEncoder();
	return 0;
}

void* CEnginePCA9685EncMX1508_moveDistance(void *engine) {

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	CEnginePCA9685EncMX1508 *currentEngine = (CEnginePCA9685EncMX1508*) engine;
	pthread_cleanup_push(CEnginePCA9685EncMX1508_moveCleanup, (void *)&(currentEngine->m_isrMutex));
	currentEngine->startMoving();
	while ((currentEngine->m_targetDistance - currentEngine->m_currentDistance)
			> 0.2 && currentEngine->isStopped() == 0) {
		pthread_mutex_lock(&(currentEngine->m_isrMutex));
		pthread_cond_wait(&(currentEngine->m_isrCond), &(currentEngine->m_isrMutex));
		pthread_mutex_unlock(&(currentEngine->m_isrMutex));
		currentEngine->m_currentDistance = currentEngine->m_encoderCount
				/ currentEngine->m_ppi;
	}
	if (currentEngine->isStopped() == 0) {
		currentEngine->breakEngine();
	}
	currentEngine->m_goTh = 0;
	if (currentEngine->isStopped() < 2) {
		currentEngine->m_moveBarrier->sync();
	} else if (currentEngine->isStopped() == 2) {
		currentEngine->m_moveBarrier->reset();
	}
	pthread_cleanup_pop(0);
	return 0;
}

void CEnginePCA9685EncMX1508::moveWOEncoder(int direction) {
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Moving in direction ");
		if (direction < 0)
			message +="backward";
		else if (direction > 0)
			message +="forward";
		message +="\n";
		m_logger->debug(message);
	}
	m_actualPower = m_enginePower;
	m_requestedDirection = direction;
	pthread_create(&m_goTh, &m_goThAttr, CEnginePCA9685EncMX1508_moveWOEncoder, this);
}

void CEnginePCA9685EncMX1508::moveDistance(float distance) {
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Moving ");
		message +=std::to_string(distance);
		message +="\n";
		m_logger->debug(message);
	}
	startEncoder();
	m_requestedDistance = distance;
	m_currentDistance = 0.0;
	m_actualPower = m_enginePower;
	m_targetDistance = distance;
	if (m_targetDistance < 0) {
		m_targetDistance = -m_targetDistance;
	}
	if (m_targetDistance <= lowPowerDistance) {
		m_actualPower = m_enginePower / 2;
		if (m_actualPower < minEnginePower) {
			m_actualPower = minEnginePower;
		}
	}
	m_encoderCount = 0;
	pthread_create(&m_goTh, &m_goThAttr, CEnginePCA9685EncMX1508_moveDistance, this);
}

void* CEnginePCA9685EncMX1508_moveEncoderNr(void *engine) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	CEnginePCA9685EncMX1508 *currentEngine = (CEnginePCA9685EncMX1508*) engine;
	pthread_cleanup_push(CEnginePCA9685EncMX1508_moveCleanup, (void *)&(currentEngine->m_isrMutex));
	currentEngine->startMoving();
	while (currentEngine->m_targetEncoder > currentEngine->m_encoderCount && currentEngine->isStopped() == 0) {
		pthread_mutex_lock(&(currentEngine->m_isrMutex));
		pthread_cond_wait(&(currentEngine->m_isrCond), &(currentEngine->m_isrMutex));
		pthread_mutex_unlock(&(currentEngine->m_isrMutex));
	}
	if (currentEngine->isStopped() == 0) {
		currentEngine->breakEngine();
	}
	currentEngine->m_goTh = 0;
	currentEngine->m_moveBarrier->sync();
	pthread_cleanup_pop(0);
	return 0;
}

void CEnginePCA9685EncMX1508::moveEncoderNr(unsigned long encoderNr, int direction) {
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("Move with encoder nr ");
		message +=std::to_string(encoderNr);
		message +="\n";
		m_logger->debug(message);
	}
	startEncoder();
	m_targetEncoder = encoderNr;
	m_encoderCount = 0;
	m_requestedDistance = direction;
	m_actualPower = m_enginePower;
	pthread_create(&m_goTh, &m_goThAttr, CEnginePCA9685EncMX1508_moveEncoderNr, this);
}

float CEnginePCA9685EncMX1508::getActualDistance() {
	return m_currentDistance;
}
