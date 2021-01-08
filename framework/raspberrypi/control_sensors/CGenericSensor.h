/*
 * CGenericSensor.h
 *
 *  Created on: Oct 12, 2020
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

#ifndef CONTROL_SENSORS_CGENERICSENSOR_H_
#define CONTROL_SENSORS_CGENERICSENSOR_H_
#include <CLogger.h>
class CGenericSensor {
public:
	CGenericSensor(CLogger *logger);
	virtual ~CGenericSensor();
	virtual int getDistance() = 0;
	/* return 1 if is sweep sensor otherwise 0 (if is fix) */
	virtual int isSweepSensor() = 0;
	virtual void movePosition(int degree) = 0;
	virtual int getPosition() = 0;
	/* -1 left 0 center 1 right */
	virtual int getRelativePosition() = 0;
	virtual int getMaxLeftPosition() = 0;
	virtual int getMaxRightPosition() = 0;
	virtual int getCenterPosition() = 0;
	virtual int isCollisionSensor() = 0;
	virtual void registerCollisionCallback(int stopDistance, void (*callbackRoutine)(void *,CGenericSensor *), void *instance) = 0;
	virtual void unregisterCollisionCallback() = 0;
	virtual void restartCollisionCallback() = 0;
	virtual std::string getDebugInformation() = 0;
protected:
	CLogger *m_logger;
};

#endif /* CONTROL_SENSORS_CGENERICSENSOR_H_ */
