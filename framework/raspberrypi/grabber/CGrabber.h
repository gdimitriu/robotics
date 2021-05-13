/*
 * CGrabber.h
 *
 *  Created on: Dec 22, 2020
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

#ifndef GRABBER_CGRABBER_H_
#define GRABBER_CGRABBER_H_

#include <Adafruit_PWMServoDriver.h>
#include <CLogger.h>

class CGrabber {
public:
	CGrabber(Adafruit_PWMServoDriver *pwmDriver, int position, int pin, int closePulse, int openPulse, CLogger *logger);
	virtual ~CGrabber();
	std::string getDebugInformation();
	int getPosition();
	void openClaw();
	void closeClaw();
protected:
	Adafruit_PWMServoDriver *m_pwmDriver;
	CLogger *m_logger;
	int m_pin;
	int m_closePulse;
	int m_openPulse;
	int m_position;
};

#endif /* GRABBER_CGRABBER_H_ */
