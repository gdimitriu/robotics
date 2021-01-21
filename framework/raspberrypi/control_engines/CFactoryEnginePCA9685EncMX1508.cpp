/*
 * CFactoryEnginePCA9685EncMX1508.cpp
 *
 *  Created on: Oct 5, 2020
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
#include "CFactoryEnginePCA9685EncMX1508.h"
#include <Adafruit_PWMServoDriver.h>
#include <CSettingLoading.h>

CFactoryEnginePCA9685EncMX1508::CFactoryEnginePCA9685EncMX1508(CSettingLoading *settingsLoader, Adafruit_PWMServoDriver *pwmDriver, CLogger *logger)
	: CFactoryEngine(logger){
	m_settingLoader = settingsLoader;
	m_pwmDriver = pwmDriver;
}

CFactoryEnginePCA9685EncMX1508::~CFactoryEnginePCA9685EncMX1508() {
}

CEngineWithEncoder **CFactoryEnginePCA9685EncMX1508::createEngines(unsigned int &enginesNr) {
	unsigned int engineNr = 0;
	unsigned int port, engPin1, engPin2;
	int edge;
	float ppi;
	CEngineWithEncoder ** engines = NULL;
	sscanf(m_settingLoader->getLine(), "%d", &enginesNr);
	if (enginesNr <= 0) {
		return NULL;
	}
	engines = new CEngineWithEncoder*[enginesNr];
	for (int i = 0; i < enginesNr; i++) {
		sscanf(m_settingLoader->getLine(), "%u %u %d %u %u %f",&engineNr, &port, &edge, &engPin1, &engPin2, &ppi);
		engines[i] = new CEnginePCA9685EncMX1508(engineNr, port, edge, engPin1, engPin2);
		engines[i]->setLogger(m_logger);
		((CEnginePCA9685EncMX1508 *)engines[i])->setPWMDriver(m_pwmDriver);
		((CEnginePCA9685EncMX1508 *)engines[i])->setPPI(ppi);
		CLogger *logger = m_settingLoader->getLogger();
		if (logger != NULL && logger->isInfo()) {
			logger->info(engines[i]->getDebugInformation());
		}
	}
	unsigned int intValue;
	sscanf(m_settingLoader->getLine(), "%u", &intValue);
	engines[0]->setMinEnginePower(intValue);
	sscanf(m_settingLoader->getLine(), "%u", &intValue);
	engines[0]->setMaxEnginePower(intValue);
	sscanf(m_settingLoader->getLine(), "%ul", &intValue);
	engines[0]->setLowPowerDistance(intValue);
	return engines;
}

void *CFactoryEnginePCA9685EncMX1508::getPWMDriver() {
	return m_pwmDriver;
}
