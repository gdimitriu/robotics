/*
 * CFactoryHCSR04SweepPCA9685.h
 *
 *  Created on: Nov 25, 2020
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

#ifndef CONTROL_SENSORS_CFACTORYHCSR04SWEEPPCA9685_H_
#define CONTROL_SENSORS_CFACTORYHCSR04SWEEPPCA9685_H_
#include <Adafruit_PWMServoDriver.h>
#include <CSettingLoading.h>
#include <CFactorySensor.h>
#include <CGenericSensor.h>

class CFactoryHCSR04SweepPCA9685: public CFactorySensor {
public:
	CFactoryHCSR04SweepPCA9685(CSettingLoading *settingsLoader, Adafruit_PWMServoDriver *pwmDriver, CLogger *logger);
	virtual ~CFactoryHCSR04SweepPCA9685();
	virtual CGenericSensor *createSensor();
private:
	CSettingLoading *m_settingsLoader;
	Adafruit_PWMServoDriver *m_pwmDriver;
};

#endif /* CONTROL_SENSORS_CFACTORYHCSR04SWEEPPCA9685_H_ */
