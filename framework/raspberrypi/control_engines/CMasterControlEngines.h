/*
 * CMasterControlEngines.h
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

#ifndef CONTROL_ENGINES_CMASTERCONTROLENGINES_H_
#define CONTROL_ENGINES_CMASTERCONTROLENGINES_H_

#include <CFactoryEngine.h>
#include <CLogger.h>
#include <Adafruit_PWMServoDriver.h>
#include <pthread.h>
#include "CResetableBarrier.h"

class CMasterControlEngines {
public:
	CMasterControlEngines(char *configFile, CLogger *settingLogger,Adafruit_PWMServoDriver *pwmDriver);
	virtual ~CMasterControlEngines();
	/* return the pwm driver */
	virtual void* getPWMDriver();
	virtual unsigned int getEnginesNr();
	/* move of specific distance in cm + is forward - is backward */
	virtual void moveDistance(float distance);
	/* get the encoder value */
	virtual unsigned long* getEncoders();
	/* get the current power of the engine */
	virtual unsigned int getEnginePower();
	/* set the engine current power */
	virtual void setEnginePower(unsigned int power);
	virtual void dumpInfo(CLogger *logger);
	virtual void breakEngines(int type = 0);
	void setMoveBarrier(CResetableBarrier *moveBarrier);
	/*
	 * encoderNr is positive for right and negative for left
	 */
	virtual void rotate(int encoderNr);
	virtual void moveWOEncoder(int direction);
	virtual void rotateWOEncoder(int direction);
	/*
	 * get the actual distance that the droid had move
	*/
	float getActualDistance();
	/*
	 * set the maximum engine power
	 */
	void setMaxEnginePower(unsigned int newPower);
	/*
	 * set the minimum engine power
	 */
	void setMinEnginePower(unsigned int newPower);
	/*
	 * get the maximum engine power
	*/
	unsigned int getMaxEnginePower();
	/*
	 * get the minimum engine power
	*/
	unsigned int getMinEnginePower();
	/*
	 * get the low power distance
	*/
	unsigned long getLowPowerDistance();
	/*
	 * set the low power distance
	*/
	void setLowPowerDistance(unsigned long distance);

protected:
	CEngineWithEncoder **m_engines;
	CFactoryEngine *m_factory;
	unsigned int m_enginesNr;
	CLogger *m_logger;
	int *m_leftEnginesIndex;
	int *m_rightEnginesIndex;
	pthread_barrier_t m_startBarrier;
};

#endif /* CONTROL_ENGINES_CMASTERCONTROLENGINES_H_ */
