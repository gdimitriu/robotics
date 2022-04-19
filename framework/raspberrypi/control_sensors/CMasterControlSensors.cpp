/*
 * CMasterControlSensors.cpp
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

#include "CMasterControlSensors.h"
#include <CFactorySensors.h>
#include <CSettingLoading.h>
#include <CLogger.h>
#include <CLoggerStdout.h>
#include <CLoggerBleHC5.h>
#include <string.h>
#include <CFactoryLogger.h>

CMasterControlSensors::CMasterControlSensors(char *configFile,
		CLogger *settingLogger, Adafruit_PWMServoDriver *pwmDriver) {
	CSettingLoading *settings = new CSettingLoading(configFile, settingLogger);
	CFactoryLogger *loggerFactory = new CFactoryLogger(settings);
	m_logger = loggerFactory->createLogger(settingLogger);
	CFactorySensors *factory = new CFactorySensors(settings, pwmDriver);
	unsigned int sensorsNr;
	CGenericSensor **sensors = factory->createSensors(sensorsNr);
	delete loggerFactory;
	delete factory;
	delete settings;
	m_forwardSensorsNr = 0;
	for(int i = 0; i < sensorsNr; i++) {
		if (sensors[i]->getPosition() == 0)
			m_forwardSensorsNr++;
	}
	m_forwardSensors = new CGenericSensor*[m_forwardSensorsNr];
	for (int i = 0, j = 0 ; i < sensorsNr; i++) {
		if (sensors[i]->getPosition() == 0) {
			m_forwardSensors[j] = sensors[i];
			j++;
		}
	}

	m_hasFrontCallback = 0;
	m_backwardSensorsNr = 0;
	for(int i = 0; i < sensorsNr; i++) {
		if (sensors[i]->getPosition() == 180)
			m_backwardSensorsNr++;
	}
	m_backwardSensors = new CGenericSensor*[m_backwardSensorsNr];
	for (int i = 0, j = 0 ; i < sensorsNr; i++) {
		if (sensors[i]->getPosition() == 180) {
			m_backwardSensors[j] = sensors[i];
			j++;
		}
	}
	m_hasBackCallback = 0;
	delete sensors;
}

CMasterControlSensors::~CMasterControlSensors() {
	for (int i = 0; i < m_forwardSensorsNr; i++) {
		delete m_forwardSensors[i];
	}
	delete[] m_forwardSensors;
	m_forwardSensorsNr = 0;
	for (int i = 0; i < m_backwardSensorsNr; i++) {
		delete m_backwardSensors[i];
	}
	delete[] m_backwardSensors;
	m_backwardSensorsNr = 0;
	if (m_logger != NULL) {
		delete m_logger;
		m_logger = NULL;
	}
}

int CMasterControlSensors::lookCenter(int position) {
	if (m_forwardSensorsNr > 0) {
		int minValue = 4000;//4m
		int currentValue;
		for (int i = 0; i < m_forwardSensorsNr; i++) {
			m_forwardSensors[i]->movePosition(0);
			currentValue = m_forwardSensors[i]->getDistance();
			if (currentValue > 0 && currentValue < minValue) {
				minValue = currentValue;
			}
		}
		return minValue;
	} else {
		return -1;
	}
}

int CMasterControlSensors::registerCollisionCallback(int stopDistance,int position, void (*callbackRoutine)(void *,CGenericSensor *), void *instance) {
	if (position == 0) { //front sensors
		for (int i = 0; i < m_forwardSensorsNr; i++) {
			if (m_forwardSensors[i]->isCollisionSensor()) {
				m_forwardSensors[i]->registerCollisionCallback(stopDistance, callbackRoutine, instance);
			}
		}
	}
	if (position == 180) { //back sensors
		for (int i = 0; i < m_backwardSensorsNr; i++) {
			if (m_backwardSensors[i]->isCollisionSensor()) {
				m_backwardSensors[i]->registerCollisionCallback(stopDistance, callbackRoutine, instance);
			}
		}
	}
	return 0;
}

int CMasterControlSensors::unregisterCollisionCallback(int position) {
	if (position == 0) { //front sensors
		for (int i = 0; i < m_forwardSensorsNr; i++) {
			if (m_forwardSensors[i]->isCollisionSensor()) {
				m_forwardSensors[i]->unregisterCollisionCallback();
			}
		}
	}
	if (position == 180) { //back sensors
		for (int i = 0; i < m_backwardSensorsNr; i++) {
			if (m_backwardSensors[i]->isCollisionSensor()) {
				m_backwardSensors[i]->unregisterCollisionCallback();
			}
		}
	}
	return 0;
}

int CMasterControlSensors::lookLeft(int position) {
	int minValue = 4000; //4 m
	int value = 0;
	if (position == 1) {
		for (int i = 0; i <  m_forwardSensorsNr; i++) {
			if (m_forwardSensors[i]->isSweepSensor()) {
				m_forwardSensors[i]->movePosition(m_forwardSensors[i]->getMaxLeftPosition());
				value = m_forwardSensors[i]->getDistance();
				m_forwardSensors[i]->movePosition(0);
				if (value > 0 && value < minValue) {
					minValue = value;
				}
			}
		}
	}
	return minValue;
}

int CMasterControlSensors::lookRight(int position) {
	int minValue = 4000; //4 m
	int value = 0;
	if (position == 1) {
		for (int i = 0; i < m_forwardSensorsNr; i++) {
			if (m_forwardSensors[i]->isSweepSensor()) {
				m_forwardSensors[i]->movePosition(m_forwardSensors[i]->getMaxRightPosition());
				value = m_forwardSensors[i]->getDistance();
				m_forwardSensors[i]->movePosition(0);
				if (value > 0 && value < minValue) {
					minValue = value;
				}
			}
		}
	}
	return minValue;
}
