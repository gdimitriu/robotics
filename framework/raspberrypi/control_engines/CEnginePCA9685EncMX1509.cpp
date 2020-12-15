/*
 * CEnginePCA9685EncMX1509.cpp
 * Engine for raspberry Pi Zero with PCA9865 and MX1509 with encoders
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

#include "CEnginePCA9685EncMX1509.h"

#include <iostream>
#include <string>
#include <sched.h>

unsigned int CEnginePCA9685EncMX1509::maxEnginePower = 4000;
unsigned int CEnginePCA9685EncMX1509::minEnginePower = 2000;
unsigned long CEnginePCA9685EncMX1509::lowPowerDistance = 300;

CEnginePCA9685EncMX1509::~CEnginePCA9685EncMX1509() {
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, 0, this);
	breakEngine();
	if (m_goTh > 0)
		pthread_cancel(m_goTh);
	pthread_attr_destroy(&m_goThAttr);
	pthread_cond_broadcast(&m_isrCond);
//	pthread_cond_destroy(&m_isrCond);
	pthread_mutex_destroy(&m_isrMutex);
}

CEnginePCA9685EncMX1509::CEnginePCA9685EncMX1509(unsigned int engineNr, unsigned int port, int edge, unsigned int engPin1, unsigned int engPin2) :
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
}

void CEnginePCA9685EncMX1509::setMaxEnginePower(unsigned int newPower) {
	this->maxEnginePower = newPower;
}

void CEnginePCA9685EncMX1509::setMinEnginePower(unsigned int newPower) {
	this->minEnginePower = newPower;
}

unsigned int CEnginePCA9685EncMX1509::getMaxEnginePower() {
	return maxEnginePower;
}

unsigned int CEnginePCA9685EncMX1509::getMinEnginePower() {
	return minEnginePower;
}

unsigned long CEnginePCA9685EncMX1509::getLowPowerDistance() {
	return lowPowerDistance;
}

void CEnginePCA9685EncMX1509::setLowPowerDistance(unsigned long distance) {
	lowPowerDistance = distance;
}

void CEnginePCA9685EncMX1509::setPWMDriver(Adafruit_PWMServoDriver *driver) {
	this->m_pwmDriver = driver;
}

void *CEnginePCA9685EncMX1509::getPWMDriver() {
	return m_pwmDriver;
}
unsigned int CEnginePCA9685EncMX1509::getEngineNr() {
	return m_engineNr;
}

void CEnginePCA9685EncMX1509::startEncoder() {
	m_encoderCount = 0;
	gpioSetMode(m_encPort, PI_INPUT);
	gpioSetPullUpDown(m_encPort, PI_PUD_UP);
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, CEnginePCA9685EncMX1509::ISRMain, this);
}

void CEnginePCA9685EncMX1509::stopEncoder() {
	gpioSetISRFuncEx(m_encPort, m_encEdge, -1, 0, this);
}

void CEnginePCA9685EncMX1509::clearEncoder() {
	m_encoderCount = 0;
}

unsigned long CEnginePCA9685EncMX1509::getEncoder() {
	return m_encoderCount;
}

void CEnginePCA9685EncMX1509::ISRMain(int gpio, int level, uint32_t tick,
		void *holder) {
	((CEnginePCA9685EncMX1509*) holder)->encISR(gpio, level, tick);
}

void CEnginePCA9685EncMX1509::encISR(int gpio, int level, uint32_t tick) {
	m_encoderCount++;
	pthread_mutex_lock(&m_isrMutex);
	pthread_cond_signal(&m_isrCond);
	pthread_mutex_unlock(&m_isrMutex);
	sched_yield();
}

void CEnginePCA9685EncMX1509::setEnginePower(unsigned int power) {
	if (power > maxEnginePower) {
		m_enginePower = maxEnginePower;
	} else if (power < minEnginePower) {
		m_enginePower = minEnginePower;
	} else {
		m_enginePower = power;
	}
}

unsigned int CEnginePCA9685EncMX1509::getEnginePower() {
	return m_enginePower;
}

void CEnginePCA9685EncMX1509::breakEngine() {
	stopEncoder();
	m_pwmDriver->setPWM(m_enginePin1, 0, 4095);
	m_pwmDriver->setPWM(m_enginePin2, 0, 4095);
}

void CEnginePCA9685EncMX1509::coastEngine() {
	stopEncoder();
	m_pwmDriver->setPWM(m_enginePin1, 0, 0);
	m_pwmDriver->setPWM(m_enginePin2, 0, 0);
}

void CEnginePCA9685EncMX1509::setPPI(float ppi) {
	m_ppi = ppi;
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("SetPPI ");
		message +=std::to_string(ppi);
		message +=" for engine ";
		message +=std::to_string(m_engineNr);
		message +='\n';
		m_logger->debug(message);
	}
}

void CEnginePCA9685EncMX1509::dumpInfo() {
	if (m_logger != NULL) {
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
		m_logger->info(message);
	}
}

void CEnginePCA9685EncMX1509::startMoving() {
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

void* CEnginePCA9685EncMX1509_moveDistance(void *engine) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	CEnginePCA9685EncMX1509 *currentEngine = (CEnginePCA9685EncMX1509*) engine;
	currentEngine->startMoving();
	while ((currentEngine->m_targetDistance - currentEngine->m_currentDistance)
			> 0.2) {
		pthread_mutex_lock(&(currentEngine->m_isrMutex));
		pthread_cond_wait(&(currentEngine->m_isrCond), &(currentEngine->m_isrMutex));
		pthread_mutex_unlock(&(currentEngine->m_isrMutex));
		currentEngine->m_currentDistance = currentEngine->m_encoderCount
				/ currentEngine->m_ppi;
	}
	currentEngine->breakEngine();
	currentEngine->m_goTh = 0;
	currentEngine->m_moveBarrier->sync();
	return 0;
}

void CEnginePCA9685EncMX1509::moveDistance(float distance) {
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
	pthread_create(&m_goTh, &m_goThAttr, CEnginePCA9685EncMX1509_moveDistance, this);
}

void* CEnginePCA9685EncMX1509_moveEncoderNr(void *engine) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	CEnginePCA9685EncMX1509 *currentEngine = (CEnginePCA9685EncMX1509*) engine;
	currentEngine->startMoving();
	while (currentEngine->m_targetEncoder > currentEngine->m_encoderCount) {
		pthread_mutex_lock(&(currentEngine->m_isrMutex));
		pthread_cond_wait(&(currentEngine->m_isrCond), &(currentEngine->m_isrMutex));
		pthread_mutex_unlock(&(currentEngine->m_isrMutex));
	}
	currentEngine->breakEngine();
	currentEngine->m_goTh = 0;
	currentEngine->m_moveBarrier->sync();
	return 0;
}

void CEnginePCA9685EncMX1509::moveEncoderNr(unsigned long encoderNr, int direction) {
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
	pthread_create(&m_goTh, &m_goThAttr, CEnginePCA9685EncMX1509_moveEncoderNr, this);
}

float CEnginePCA9685EncMX1509::getActualDistance() {
	return m_currentDistance;
}
