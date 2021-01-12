/*
 * CFixDigitalSensor.h
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

#ifndef CONTROL_SENSORS_CFIXDIGITALSENSOR_H_
#define CONTROL_SENSORS_CFIXDIGITALSENSOR_H_

#include "CGenericSensor.h"
#include <pigpio.h>
#include <pthread.h>

class CFixDigitalSensor: public CGenericSensor {
public:
	CFixDigitalSensor(unsigned int pin, unsigned int edge, int position, int isCollision, CLogger *logger, int relativePosition);
	static void ISRMain(int gpio, int level, uint32_t tick, void *holder);
	virtual ~CFixDigitalSensor();
	virtual int getDistance();
	/* return 1 if is sweep sensor otherwise 0 (if is fix) */
	virtual int isSweepSensor();
	virtual void movePosition(int degree);
	virtual int getPosition();
	virtual int getRelativePosition();
	virtual int isCollisionSensor();
	virtual void registerCollisionCallback(int stopDistance, void (*callbackRoutine)(void*, CGenericSensor*), void *instance);
	virtual void restartCollisionCallback();
	virtual void collisionCallback();
	virtual int getMaxLeftPosition();
	virtual int getMaxRightPosition();
	virtual int getCenterPosition();
	virtual void unregisterCollisionCallback();
	virtual std::string getDebugInformation();
private:
	void collisionISR();
	int m_position;
	int m_relativePosition;
	int m_isCollision;
	pthread_t m_collisionTh;
	pthread_attr_t m_collisionThAttr;
	void *m_callbackCaller;
	void (*m_callbackRoutine)(void *,CGenericSensor *);
	unsigned int m_pin;
	unsigned int m_edge;
	pthread_mutex_t m_mutex;
	pthread_cond_t m_condition;
	unsigned int m_isDisabled;
	struct sched_param m_goSchedParam;
	pthread_mutexattr_t m_mutexAttr;
	friend void *CFixDigitalSensor_collisionDetection(void *instance);
};

#endif /* CONTROL_SENSORS_CFIXDIGITALSENSOR_H_ */
