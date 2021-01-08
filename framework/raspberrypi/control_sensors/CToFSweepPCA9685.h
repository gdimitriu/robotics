/*
 * CToFSweepPCA9685.h
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

#ifndef CONTROL_SENSORS_CTOFSWEEPPCA9685_H_
#define CONTROL_SENSORS_CTOFSWEEPPCA9685_H_

#include <CSweepPCA9685.h>
#include <mutex>

class CToFSweepPCA9685 : public CSweepPCA9685 {
public:
	CToFSweepPCA9685(int iChan, int iAddr, Adafruit_PWMServoDriver *pwmDriver, int servoPin , int servoCenter, int servoLeft, int servoRight, int position,
			int isCollision, CLogger *logger, int maxLeft, int maxRight, int relativePosition);
	virtual ~CToFSweepPCA9685();
	virtual int getDistance();
	virtual std::string getDebugInformation();
protected:
	int m_i2cChannel;
	int m_i2cAddress;
};

#endif /* CONTROL_SENSORS_CTOFSWEEPPCA9685_H_ */
