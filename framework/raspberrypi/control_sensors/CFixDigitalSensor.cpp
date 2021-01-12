/*
 * CFixDigitalSensor.cpp
 *
 *  Created on: Oct 16, 2020
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

#include "CFixDigitalSensor.h"
#include <string>
#include <pigpio.h>
#include <pthread.h>
#include <sched.h>

CFixDigitalSensor::CFixDigitalSensor(unsigned int pin, unsigned int edge,
		int position, int isCollision, CLogger *logger, int relativePosition) :
		CGenericSensor(logger) {
	m_position = position;
	m_relativePosition = relativePosition;
	m_isCollision = isCollision;
	m_callbackCaller = NULL;
	m_callbackRoutine = NULL;
	m_collisionTh = 0;
	m_pin = pin;
	m_edge = edge;

	m_isDisabled = 1;
	pthread_cond_init(&m_condition, NULL);
	pthread_attr_init(&m_collisionThAttr);
	pthread_mutexattr_init(&m_mutexAttr);
	pthread_mutexattr_setprotocol(&m_mutexAttr, PTHREAD_PRIO_INHERIT);
	pthread_mutex_init(&m_mutex, &m_mutexAttr);
	pthread_attr_setschedpolicy(&m_collisionThAttr, SCHED_FIFO);
	m_goSchedParam.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_attr_setschedparam(&m_collisionThAttr, &m_goSchedParam);
	if (isCollision) {
		gpioSetMode(m_pin, PI_INPUT);
		gpioSetPullUpDown(m_pin, PI_PUD_UP);
	}
}

CFixDigitalSensor::~CFixDigitalSensor() {
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
	}
	if (m_isCollision) {
		gpioSetISRFuncEx(m_pin, m_edge, -1, NULL, this);
	}
	pthread_cond_broadcast(&m_condition);
	pthread_mutex_destroy(&m_mutex);
	pthread_cond_destroy(&m_condition);
	pthread_attr_destroy(&m_collisionThAttr);
	pthread_mutexattr_destroy(&m_mutexAttr);
	gpioSetPullUpDown(m_pin, PI_PUD_OFF);
}

std::string CFixDigitalSensor::getDebugInformation() {
	std::string message("CFixDigitalSensor pin=");
	message += std::to_string(m_pin);
	message += " edge=";
	message += std::to_string(m_edge);
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
	switch (m_relativePosition) {
	case -1:
		message +=" in left relative\n";
		break;
	case 0:
		message +=" in center relative\n";
		break;
	case 1:
		message +=" in right relative\n";
		break;
	}
	return message;
}

void CFixDigitalSensor::ISRMain(int gpio, int level, uint32_t tick, void *holder) {
	((CFixDigitalSensor *)holder)->collisionISR();
}

int CFixDigitalSensor::getDistance() {
	return -1;
}

int CFixDigitalSensor::isSweepSensor() {
	return 0;
}

void CFixDigitalSensor::movePosition(int degree) {

}

int CFixDigitalSensor::getPosition() {
	return m_position;
}

int CFixDigitalSensor::getRelativePosition() {
	return m_relativePosition;
}

int CFixDigitalSensor::getMaxLeftPosition() {
	return m_position;
}

int CFixDigitalSensor::getMaxRightPosition() {
	return m_position;
}

int CFixDigitalSensor::getCenterPosition() {
	return m_position;
}

int CFixDigitalSensor::isCollisionSensor() {
	return m_isCollision;
}

void CFixDigitalSensor_collisionCleanup(void *arg) {
	pthread_mutex_t *pm = (pthread_mutex_t *)arg;
	pthread_mutex_unlock(pm);
}

/*
 * collision mechanism with threads
 */
void *CFixDigitalSensor_collisionDetection(void *instance) {
	CFixDigitalSensor *sensor = (CFixDigitalSensor *)instance;
	pthread_cleanup_push(CFixDigitalSensor_collisionCleanup, (void *)&(sensor->m_mutex));
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	sensor->collisionCallback();
	pthread_cleanup_pop(0);
	return 0;
}

void CFixDigitalSensor::collisionCallback() {
	pthread_mutex_lock(&m_mutex);
	pthread_cond_wait(&m_condition, &m_mutex);
	m_isDisabled = 1;
	pthread_mutex_unlock(&m_mutex);
	m_collisionTh = 0;
	if (m_isCollision) {
		gpioSetISRFuncEx(m_pin, m_edge, -1, NULL, this);
	}
	m_callbackRoutine(m_callbackCaller, this);
}
void CFixDigitalSensor::registerCollisionCallback(int stopDistance,
		void (*callbackRoutine)(void*, CGenericSensor*), void *instance) {
	if (m_collisionTh > 0)
		pthread_cancel(m_collisionTh);
	gpioSetISRFuncEx(m_pin, m_edge, -1, CFixDigitalSensor::ISRMain, this);
	m_callbackCaller = instance;
	m_callbackRoutine = callbackRoutine;
	m_isDisabled = 0;
	pthread_create(&m_collisionTh, &m_collisionThAttr, CFixDigitalSensor_collisionDetection, this);
}

void CFixDigitalSensor::collisionISR() {
	if (m_callbackRoutine != NULL && m_isDisabled == 0) {
		pthread_mutex_lock(&m_mutex);
		pthread_cond_signal(&m_condition);
		pthread_mutex_unlock(&m_mutex);
	}
}

void CFixDigitalSensor::restartCollisionCallback() {
	if (m_collisionTh > 0)
		pthread_cancel(m_collisionTh);
	pthread_create(&m_collisionTh, &m_collisionThAttr, CFixDigitalSensor_collisionDetection, this);
}

void CFixDigitalSensor::unregisterCollisionCallback() {
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
		m_collisionTh = 0;
	}
	m_callbackCaller = NULL;
	m_callbackRoutine = NULL;
	m_isDisabled = 1;
}
