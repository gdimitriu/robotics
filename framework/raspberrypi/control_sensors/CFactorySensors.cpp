/*
 * CFactorySensors.cpp
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

#include "CFactorySensors.h"
#include <CFactorySensor.h>
#include <CFactoryToFSweepPCA9685.h>
#include <CFactoryFixDigitalSensor.h>
#include <CFactoryHCSR04SweepPCA9685.h>
#include <stdio.h>
#include <string.h>

CFactorySensors::CFactorySensors(CSettingLoading *settingsLoader, Adafruit_PWMServoDriver *pwmDriver, CLogger *logger) {
	this->m_settingsLoader = settingsLoader;
	this->m_pwmDriver = pwmDriver;
	this->m_logger = logger;
}

CFactorySensors::~CFactorySensors() {

}

CGenericSensor **CFactorySensors::createSensors(unsigned int &sensorsNr) {
	CGenericSensor **sensors;
	char *type;
	sscanf(m_settingsLoader->getLine(), "%d", &sensorsNr);
	if (sensorsNr <= 0) {
		return NULL;
	}
	sensors = new CGenericSensor*[sensorsNr];
	for (int i = 0; i < sensorsNr; i++) {
		CFactorySensor *factorySensor;
		type = m_settingsLoader->getLine();
		if (strcmp("CFactoryToFSweepPCA9685", type) == 0) {
			factorySensor = new CFactoryToFSweepPCA9685(m_settingsLoader, m_pwmDriver, m_logger);
		} else if (strcmp("CFactoryFixDigitalSensor", type ) == 0) {
			factorySensor = new CFactoryFixDigitalSensor(m_settingsLoader, m_logger);
		} else if (strcmp("CFactoryHCSR04SweepPCA9685", type) == 0) {
			factorySensor = new CFactoryHCSR04SweepPCA9685(m_settingsLoader, m_pwmDriver, m_logger);
		} else {
			continue;
		}
		sensors[i] = factorySensor->createSensor();
		delete factorySensor;
	}
	return sensors;
}
