/*
 * CFixDigitalSensor.cpp
 *
 *  Created on: Oct 16, 2020
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

#include "CFixDigitalSensor.h"
#include <string>
#include <pigpio.h>
#include <pthread.h>

CFixDigitalSensor::CFixDigitalSensor(unsigned int pin, unsigned int edge,
		int position, int isCollision, CLogger *logger) :
		CGenericSensor(logger) {
	m_position = position;
	m_isCollision = isCollision;
	m_callbackCaller = NULL;
	m_callbackRoutine = NULL;
	m_collisionTh = 0;
	m_pin = pin;
	m_edge = edge;
	if (m_logger != NULL && m_logger->isDebug() == 1) {
		std::string message("CFixDigitalSensor pin=");
		message += std::to_string(pin);
		message += " edge=";
		message += std::to_string(edge);
		if (isCollision == 1) {
			message += " is collision avoidance sensor ";
		}
		switch (m_position) {
		case 0:
			message += " in front position\n";
			break;
		case 90:
			message += " in right position\n";
			break;
		case 180:
			message += " in back position\n";
			break;
		case 270:
			message += " in left position\n";
			break;
		}
		m_logger->debug(message);
	}
	m_isDisabled = 1;
	pthread_cond_init(&m_condition, NULL);
	pthread_mutex_init(&m_mutex, NULL);
	if (isCollision) {
		gpioSetMode(m_pin, PI_INPUT);
		gpioSetPullUpDown(m_pin, PI_PUD_UP);
		gpioSetISRFuncEx(m_pin, m_edge, -1, CFixDigitalSensor::ISRMain, this);
	}
	pthread_attr_init(&m_collisionThAttr);
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
	gpioSetPullUpDown(m_pin, PI_PUD_OFF);
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

/*
 * collision mechanism with threads
 */
void *CFixDigitalSensor_collisionDetection(void *instance) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	((CFixDigitalSensor *)instance)->collisionCallback();
	return 0;
}

void CFixDigitalSensor::collisionCallback() {
	pthread_mutex_lock(&m_mutex);
	pthread_cond_wait(&m_condition, &m_mutex);
	pthread_mutex_unlock(&m_mutex);
	m_callbackRoutine(m_callbackCaller, this);
	m_collisionTh = 0;
}
void CFixDigitalSensor::registerCollisionCallback(int stopDistance,
		void (*callbackRoutine)(void*, CGenericSensor*), void *instance) {
	m_callbackCaller = instance;
	m_callbackRoutine = callbackRoutine;
	m_isDisabled = 0;
	if (m_collisionTh > 0)
		pthread_cancel(m_collisionTh);
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
