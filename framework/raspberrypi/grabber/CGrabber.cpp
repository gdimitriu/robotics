/*
 * CGrabber.cpp
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

#include "CGrabber.h"

CGrabber::CGrabber(Adafruit_PWMServoDriver *pwmDriver, int position, int pin, int closePulse, int openPulse, CLogger *logger) {
	m_logger = logger;
	m_pwmDriver = pwmDriver;
	m_pin = pin;
	m_closePulse = closePulse;
	m_openPulse = openPulse;
	m_position = position;
	//put the grabber in half position
	m_pwmDriver->setPWM(m_pin, 0, (m_closePulse + m_openPulse)/2);
}

CGrabber::~CGrabber() {
	if (m_logger != NULL) {
		delete m_logger;
	}
}

std::string CGrabber::getDebugInformation() {
	std::string message("Grabber");
	switch(m_position) {
	case 0:
		message += " in front position";
		break;
	case 180:
		message += " in back position";
		break;
	}
	message += " with port ";
	message += std::to_string(m_pin);
	message += " with close pulse ";
	message += std::to_string(m_closePulse);
	message += " with open pulse ";
	message += std::to_string(m_openPulse);
	message +="\n";
	return message;
}

int CGrabber::getPosition() {
	return m_position;
}

void CGrabber::openClaw() {
	m_pwmDriver->setPWM(m_pin, 0, m_openPulse);
}

void CGrabber::closeClaw() {
	m_pwmDriver->setPWM(m_pin, 0, m_closePulse);
}

