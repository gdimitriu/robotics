/*
 * CMasterControlEngines.cpp
 *
 *  Created on: Oct 6, 2020
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

#include "CMasterControlEngines.h"
#include <CFactoryEnginePCA9685EncMX1508.h>
#include <CLogger.h>
#include <CLoggerStdout.h>
#include <CLoggerBleHC5.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <CFactoryLogger.h>
#include <CSettingLoading.h>

CMasterControlEngines::CMasterControlEngines(char *configFile,
		CLogger *settingLogger, Adafruit_PWMServoDriver *pwmDriver) {
	CSettingLoading *settingsLoading = new CSettingLoading(configFile, settingLogger);
	CFactoryLogger *loggerFactory = new CFactoryLogger(settingsLoading);
	m_logger = loggerFactory->createLogger(settingLogger);
	char *type = settingsLoading->getLine();
	if (strcmp("CFactoryEnginePCA9685EncMX1508", type) == 0) {
		m_factory = new CFactoryEnginePCA9685EncMX1508(settingsLoading, pwmDriver, m_logger);
	} else {
		m_factory = NULL;
		return;
	}
	m_engines = m_factory->createEngines(m_enginesNr);
	delete settingsLoading;
	delete loggerFactory;
	m_leftEnginesIndex = (int*) calloc(m_enginesNr / 2, sizeof(int));
	m_rightEnginesIndex = (int*) calloc(m_enginesNr / 2, sizeof(int));
	int left = 0;
	int right = 0;
	for (int i = 0; i < m_enginesNr; i++) {
		if (m_engines[i]->getEngineNr() < m_enginesNr / 2) {
			m_leftEnginesIndex[left] = i;
			left++;
		} else {
			m_rightEnginesIndex[right] = i;
			right++;
		}
	}
	pthread_barrier_init(&m_startBarrier, NULL, m_enginesNr);
	for (int i =0 ; i < m_enginesNr; i++) {
		m_engines[i]->setStartBarrier(&m_startBarrier);
	}
}

CMasterControlEngines::~CMasterControlEngines() {
	free(m_leftEnginesIndex);
	free(m_rightEnginesIndex);
	if (m_engines != NULL) {
		for (int i = 0 ; i < m_enginesNr; i++)
			delete m_engines[i];
		delete[] m_engines;
	}
	pthread_barrier_destroy(&m_startBarrier);
	if (m_logger != NULL) {
		delete m_logger;
		m_logger = NULL;
	}
	if (m_factory != NULL) {
		delete m_factory;
	}
}

void CMasterControlEngines::setMoveBarrier(CResetableBarrier *moveBarrier) {
	for (int i =0 ; i < m_enginesNr; i++) {
		m_engines[i]->setMoveBarrier(moveBarrier);
	}
}

void* CMasterControlEngines::getPWMDriver() {
	if (m_factory != NULL)
		return m_factory->getPWMDriver();
	else
		return NULL;
}

unsigned int CMasterControlEngines::getEnginesNr() {
	return m_enginesNr;
}

unsigned int CMasterControlEngines::getEnginePower() {
	if (m_enginesNr > 1) {
		return m_engines[0]->getEnginePower();
	}
	return 0;
}

void CMasterControlEngines::setEnginePower(unsigned int power) {
	for (int i = 0; i < m_enginesNr; i++) {
		m_engines[i]->setEnginePower(power);
	}
}

void CMasterControlEngines::setMaxEnginePower(unsigned int newPower) {
	if (m_enginesNr > 0)
		m_engines[0]->setMaxEnginePower(newPower);
}

void CMasterControlEngines::setMinEnginePower(unsigned int newPower) {
	if (m_enginesNr > 0)
		m_engines[0]->setMinEnginePower(newPower);
}

unsigned int CMasterControlEngines::getMaxEnginePower() {
	if (m_enginesNr > 0)
		return m_engines[0]->getMaxEnginePower();
	return 0;
}

unsigned int CMasterControlEngines::getMinEnginePower() {
	if (m_enginesNr > 0)
		return m_engines[0]->getMinEnginePower();
	return 0;
}

unsigned long  CMasterControlEngines::getLowPowerDistance() {
	if (m_enginesNr > 0)
		return m_engines[0]->getLowPowerDistance();
	return 0;
}

void CMasterControlEngines::setLowPowerDistance(unsigned long distance) {
	if (m_enginesNr > 0)
		m_engines[0]->setLowPowerDistance(distance);
}

unsigned long* CMasterControlEngines::getEncoders() {
	unsigned long *encoders = (unsigned long*) calloc(m_enginesNr,
			sizeof(unsigned long));
	for (int i = 0; i < m_enginesNr; i++) {
		encoders[i] = m_engines[i]->getEncoder();
	}
	return encoders;
}

void CMasterControlEngines::moveDistance(float distance) {
	for (int i = 0; i < m_enginesNr; i++) {
		m_engines[i]->moveDistance(distance);
	}
}

void CMasterControlEngines::moveWOEncoder(int direction) {
	for (int i = 0; i < m_enginesNr; i++) {
		m_engines[i]->moveWOEncoder(direction);
	}
}

void CMasterControlEngines::dumpInfo(CLogger *logger) {
	for (int i = 0; i < m_enginesNr; i++) {
		m_engines[i]->dumpInfo(logger);
	}
}

void CMasterControlEngines::breakEngines(int type) {
	for (int i = 0; i < m_enginesNr; i++) {
		m_engines[i]->breakEngine(type);
	}
}

void CMasterControlEngines::rotate(int encoderNr) {
	if (encoderNr < 0) { //left
		for (int i = 0; i < m_enginesNr / 2; i++) {
			m_engines[m_leftEnginesIndex[i]]->moveEncoderNr(-encoderNr, -1);
			m_engines[m_rightEnginesIndex[i]]->moveEncoderNr(-encoderNr, 1);
		}
	} else 	if (encoderNr > 0) { //right
		for (int i = 0; i < m_enginesNr / 2; i++) {
			m_engines[m_leftEnginesIndex[i]]->moveEncoderNr(encoderNr, 1);
			m_engines[m_rightEnginesIndex[i]]->moveEncoderNr(encoderNr, -1);
		}
	}
}

void CMasterControlEngines::rotateWOEncoder(int direction) {
	if (direction < 0) { //left
		for (int i = 0; i < m_enginesNr / 2; i++) {
			m_engines[m_leftEnginesIndex[i]]->moveWOEncoder(-1);
			m_engines[m_rightEnginesIndex[i]]->moveWOEncoder(1);
		}
	} else 	if (direction > 0) { //right
		for (int i = 0; i < m_enginesNr / 2; i++) {
			m_engines[m_leftEnginesIndex[i]]->moveWOEncoder(1);
			m_engines[m_rightEnginesIndex[i]]->moveWOEncoder(-1);
		}
	}
}

float CMasterControlEngines::getActualDistance() {
	float actualMaxDistance = 0;
	float engineDistance;
	for (int i = 0; i < m_enginesNr; i++) {
		engineDistance = m_engines[i]->getActualDistance();
		if (actualMaxDistance < engineDistance) {
			actualMaxDistance = engineDistance;
		}
	}
	return actualMaxDistance;
}
