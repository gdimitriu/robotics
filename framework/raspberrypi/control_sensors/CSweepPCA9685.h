/*
 * CSweepPCA9685.h
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

#ifndef CONTROL_SENSORS_CSWEEPPCA9685_H_
#define CONTROL_SENSORS_CSWEEPPCA9685_H_
#include <Adafruit_PWMServoDriver.h>
#include <CGenericSensor.h>
#include <CLogger.h>

class CSweepPCA9685 : public CGenericSensor{
public:
	CSweepPCA9685(Adafruit_PWMServoDriver *pwmDriver, int servoPin , int servoCenter, int servoLeft, int servoRight, int position,
			int isCollision, CLogger *logger, int maxLeft, int maxRight, int relativePosition);
	virtual ~CSweepPCA9685();
	virtual int isSweepSensor();
	virtual void movePosition(int degree);
	/* 0 front 180 back */
	virtual int getPosition();
	/* -1 left 0 center 1 right */
	virtual int getRelativePosition();
	virtual int getMaxLeftPosition();
	virtual int getMaxRightPosition();
	virtual int getCenterPosition();
	virtual int isCollisionSensor();
	virtual void registerCollisionCallback(int stopDistance, void (*callbackRoutine)(void *,CGenericSensor *), void *instance);
	virtual void restartCollisionCallback();
	virtual int getStopDistance();
	virtual void unregisterCollisionCallback();
protected:
	virtual void callCollisionCallback();
	Adafruit_PWMServoDriver *m_pwmDriver;
	int m_servoPin;
	int m_servoCenter;
	int m_servoLeft;
	int m_servoRight;
	int m_position;
	int m_relativePosition;
	int m_servoMaxRight;
	int m_servoMaxLeft;
	int m_isCollision;
	int m_stopDistance;
private:
	friend void *CSweepPCA9685_collisionDetection(void *instance);
	pthread_t m_collisionTh;
	pthread_attr_t m_collisionThAttr;
	void *m_callbackCaller;
	void (*m_callbackRoutine)(void *,CGenericSensor *);
};

#endif /* CONTROL_SENSORS_CSWEEPPCA9685_H_ */
