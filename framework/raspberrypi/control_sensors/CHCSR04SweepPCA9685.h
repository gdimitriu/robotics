/*
 * CHCSR04SweepPCA9685.h
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

#ifndef CONTROL_SENSORS_CHCSR04SWEEPPCA9685_H_
#define CONTROL_SENSORS_CHCSR04SWEEPPCA9685_H_

#include <CSweepPCA9685.h>

class CHCSR04SweepPCA9685: public CSweepPCA9685 {
public:
	CHCSR04SweepPCA9685(Adafruit_PWMServoDriver *pwmDriver, int servoPin, int servoCenter,
			int servoLeft, int servoRight, int position, int isCollision,
			CLogger *logger, int maxLeft, int maxRight, int echoPort, int trigPort,int relativePosition);
	virtual ~CHCSR04SweepPCA9685();
	virtual int getDistance();
	virtual std::string getDebugInformation();
private:
	int m_echoPort;
	int m_trigPort;
};

#endif /* CONTROL_SENSORS_CHCSR04SWEEPPCA9685_H_ */
