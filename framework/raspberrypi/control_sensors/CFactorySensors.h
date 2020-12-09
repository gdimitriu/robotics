/*
 * CFactorySensors.h
 *
 *  Created on: Oct 13, 2020
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

#ifndef CONTROL_SENSORS_CFACTORYSENSORS_H_
#define CONTROL_SENSORS_CFACTORYSENSORS_H_
#include <Adafruit_PWMServoDriver.h>
#include <CSettingLoading.h>
#include <CFactorySensor.h>
#include <CGenericSensor.h>

class CFactorySensors {
public:
	CFactorySensors(CSettingLoading *settingsLoader, Adafruit_PWMServoDriver *pwmDriver, CLogger *logger);
	virtual ~CFactorySensors();
	virtual CGenericSensor **createSensors(unsigned int &sensorsNr);
private:
	CSettingLoading *m_settingsLoader;
	Adafruit_PWMServoDriver *m_pwmDriver;
	CLogger *m_logger;
};

#endif /* CONTROL_SENSORS_CFACTORYSENSORS_H_ */
