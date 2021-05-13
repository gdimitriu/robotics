/*
 * CGrabberController.h
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

#ifndef GRABBER_CGRABBERCONTROLLER_H_
#define GRABBER_CGRABBERCONTROLLER_H_

#include <Adafruit_PWMServoDriver.h>
#include <CMasterControlSensors.h>
#include <CLogger.h>
#include <CGrabber.h>

class CGrabberController {
public:
	CGrabberController(char *configFile, CLogger *settingLogger, Adafruit_PWMServoDriver *pwmDriver, CMasterControlSensors *sensors);
	virtual ~CGrabberController();
	void openClaw(int position);
	void closeClaw(int position);
protected:
	CGrabber *m_grabber;
	CMasterControlSensors *m_controlSensors;
};

#endif /* GRABBER_CGRABBERCONTROLLER_H_ */
