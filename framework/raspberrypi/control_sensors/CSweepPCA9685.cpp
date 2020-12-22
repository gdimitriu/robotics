/*
 * CSweepPCA9685.cpp
 *
 *  Created on: Nov 24, 2020
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

#include "CSweepPCA9685.h"

CSweepPCA9685::CSweepPCA9685(Adafruit_PWMServoDriver *pwmDriver, int servoPin , int servoCenter, int servoLeft, int servoRight, int position,
		int isCollision, CLogger *logger, int maxLeft, int maxRight, int relativePosition) : CGenericSensor(logger){
	m_pwmDriver = pwmDriver;
	m_servoPin = servoPin;
	m_servoCenter = servoCenter;
	m_servoLeft = servoLeft;
	m_servoRight = servoRight;
	m_position = position;
	m_servoMaxLeft = maxLeft;
	m_servoMaxRight = maxRight;
	m_pwmDriver->setPWM(servoPin, 0, m_servoCenter);
	m_isCollision = isCollision;
	m_collisionTh = 0;
	m_stopDistance = -1;
	m_callbackRoutine = 0;
	m_callbackCaller = 0;
	m_relativePosition = relativePosition;
	struct sched_param goSchedParam;
	pthread_attr_setschedpolicy(&m_collisionThAttr, SCHED_FIFO);
	goSchedParam.__sched_priority = sched_get_priority_max(SCHED_FIFO);
	pthread_attr_init(&m_collisionThAttr);
}

CSweepPCA9685::~CSweepPCA9685() {
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
	}
	pthread_attr_destroy(&m_collisionThAttr);
}

void CSweepPCA9685::movePosition(int degree) {
	if (degree == 0) {
		m_pwmDriver->setPWM(m_servoPin, 0, m_servoCenter);
		delay(m_servoCenter);
	} else if (degree == m_servoMaxLeft) {
		m_pwmDriver->setPWM(m_servoPin, 0, m_servoLeft);
		delay(m_servoLeft);
	} else if (degree == m_servoMaxRight){
		m_pwmDriver->setPWM(m_servoPin, 0, m_servoRight);
		delay(m_servoLeft);
	}
}

int CSweepPCA9685::isSweepSensor() {
	return 1;
}

int CSweepPCA9685::getPosition() {
	return m_position;
}

int CSweepPCA9685::getRelativePosition() {
	return m_relativePosition;
}

int CSweepPCA9685::getMaxLeftPosition() {
	return m_servoMaxLeft;
}

int CSweepPCA9685::getMaxRightPosition() {
	return m_servoMaxRight;
}

int CSweepPCA9685::getCenterPosition() {
	return m_servoCenter;
}


/*----------
 *  CollisionDetection mechanism
 -----------*/
int CSweepPCA9685::isCollisionSensor() {
	return m_isCollision;
}

void* CSweepPCA9685_collisionDetection(void *instance) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	CSweepPCA9685 *currentInstance = (CSweepPCA9685*) instance;
	while (currentInstance->getStopDistance() < currentInstance->getDistance()) {
		usleep(1000);
	}
	currentInstance->callCollisionCallback();
	currentInstance->m_collisionTh = 0;
	return 0;
}

int CSweepPCA9685::getStopDistance() {
	return m_stopDistance;
}

void CSweepPCA9685::callCollisionCallback() {
	m_callbackRoutine(m_callbackCaller, this);
}

void CSweepPCA9685::registerCollisionCallback(int stopDistance,
		void (*callbackRoutine)(void*, CGenericSensor*), void *instance) {
	m_stopDistance = stopDistance;
	m_callbackRoutine = callbackRoutine;
	m_callbackCaller = instance;
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
	}
	pthread_create(&m_collisionTh, &m_collisionThAttr, CSweepPCA9685_collisionDetection, this);
}

void CSweepPCA9685::restartCollisionCallback() {
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
	}
	pthread_create(&m_collisionTh, &m_collisionThAttr, CSweepPCA9685_collisionDetection, this);
}

void CSweepPCA9685::unregisterCollisionCallback() {
	if (m_collisionTh > 0) {
		pthread_cancel(m_collisionTh);
		m_collisionTh = 0;
	}
	m_stopDistance = 0;
	m_callbackRoutine = 0;
	m_callbackCaller = 0;
}
