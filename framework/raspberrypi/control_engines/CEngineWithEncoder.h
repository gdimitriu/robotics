/*
 * CEngineWithEncoder.h
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

#ifndef CONTROL_ENGINES_CENGINEWITHENCODER_H_
#define CONTROL_ENGINES_CENGINEWITHENCODER_H_

#include <CLogger.h>
#include <pthread.h>
#include "CResetableBarrier.h"

class CEngineWithEncoder {
public:
	CEngineWithEncoder();
	virtual ~CEngineWithEncoder();
	virtual void dumpInfo(CLogger *logger) = 0;
	/* get the engine number */
	virtual unsigned int getEngineNr() = 0;
	/* coast the engine, engine will have no brake */
	virtual void coastEngine() = 0;
	/* break the engine */
	virtual void breakEngine(int type = 0) = 0;
	/* set PPI for the encoder */
	virtual void setPPI(float ppi) = 0;
	/* get the encoder value */
	virtual unsigned long getEncoder() = 0;
	/* get the current power of the engine */
	virtual unsigned int getEnginePower() = 0;
	/* set the engine current power */
	virtual void setEnginePower(unsigned int power) = 0;
	/* move of specific distance in cm + is forward - is backward */
	virtual void moveDistance(float distance) = 0;
	/*
	 * This will be use to rotate the droid
	 * use for left direction = -1 or right = 1
	 * encoderNr is the number of ticks to move
	 */
	virtual void moveEncoderNr(unsigned long encoderNr, int direction) = 0;
	virtual void setLogger(CLogger *logger);
	void setStartBarrier(pthread_barrier_t *barrier);
	void setMoveBarrier(CResetableBarrier *moveBarrier);
	virtual void moveWOEncoder(int direction) =0;
	/*
	 * get the actual distance that the droid had move
	 */
	virtual float getActualDistance() = 0;
	/*
	 * set the maximum engine power this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual void setMaxEnginePower(unsigned int newPower) = 0;
	/*
	 * set the minimum engine power this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual void setMinEnginePower(unsigned int newPower) = 0;
	/*
	 * get the maximum engine power this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual unsigned int getMaxEnginePower() = 0;
	/*
	 * get the minimum engine power this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual unsigned int getMinEnginePower() = 0;
	/*
	 * get the low power distance this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual unsigned long getLowPowerDistance() = 0;
	/*
	 * set the low power distance this should be called only once per all engines
	 * behind is a static variable
	*/
	virtual void setLowPowerDistance(unsigned long distance) = 0;
	/*
	 * get the debug information
	 */
	virtual std::string getDebugInformation() = 0;
protected:
	CLogger *m_logger;
	pthread_barrier_t *m_startBarrier;
	CResetableBarrier *m_moveBarrier;
};

#endif /* CONTROL_ENGINES_CENGINEWITHENCODER_H_ */
