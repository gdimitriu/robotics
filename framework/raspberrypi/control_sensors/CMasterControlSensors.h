/*
 * CMasterControlSensors.h
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

#ifndef CONTROL_SENSORS_CMASTERCONTROLSENSORS_H_
#define CONTROL_SENSORS_CMASTERCONTROLSENSORS_H_
#include <CLogger.h>
#include <Adafruit_PWMServoDriver.h>
#include <CGenericSensor.h>

class CMasterControlSensors {
public:
	CMasterControlSensors(char *configFile, CLogger *settingLogger,Adafruit_PWMServoDriver *pwmDriver);
	virtual ~CMasterControlSensors();
	/* look front center if position 1 and back if position is -1 */
	virtual int lookCenter(int position);
	/* look front left if position 1 and back if position is -1 */
	virtual int lookLeft(int position);
	/* look front right if position 1 and back if position is -1 */
	virtual int lookRight(int position);
	virtual int registerCollisionCallback(int stopDistance,int position, void (*callbackRoutine)(void *,CGenericSensor *), void *instance);
	virtual int unregisterCollisionCallback(int position);
protected:
	CLogger *m_logger;
	CGenericSensor **m_forwardSensors;
	unsigned int m_forwardSensorsNr;
	unsigned int m_hasFrontCallback;
	CGenericSensor **m_backwardSensors;
	unsigned int m_backwardSensorsNr;
	unsigned int m_hasBackCallback;
};

#endif /* CONTROL_SENSORS_CMASTERCONTROLSENSORS_H_ */
