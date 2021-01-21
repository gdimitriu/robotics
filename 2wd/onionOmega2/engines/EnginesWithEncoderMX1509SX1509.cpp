/*
 Engine driver with L298N micro and m_extender SX1509
 Copyright (C) 2020 Gabriel Dimitriu
 All rights reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "EnginesWithEncoderMX1509SX1509.h"

EnginesWithEncoderMX1509SX1509::EnginesWithEncoderMX1509SX1509(
		float t_whellRadius, long t_resolutionCoderLeft, long t_resolutionCoderRight,
		long t_countRotate1Inner, long t_countRotate1Outer,
		unsigned int t_leftEncoderPin, unsigned int t_rightEncoderPin) :
		EnginesWithEncoderMX1509(t_whellRadius, t_resolutionCoderLeft,
				t_resolutionCoderRight, t_countRotate1Inner, t_countRotate1Outer,
				t_leftEncoderPin, t_rightEncoderPin) {
	m_extender = nullptr;
	m_clockDivider = 7;
	setLeftEnginePower(1);
	setRightEnginePower(1);
}

EnginesWithEncoderMX1509SX1509::EnginesWithEncoderMX1509SX1509() {
	m_extender = nullptr;
	m_clockDivider = 7;
}

EnginesWithEncoderMX1509SX1509::~EnginesWithEncoderMX1509SX1509() {
	if (m_extender != nullptr) {
		delete m_extender;
	}
}

void EnginesWithEncoderMX1509SX1509::setClockDivider(
		unsigned char t_divider) {
	this->m_clockDivider = t_divider;
	if (m_extender != nullptr) {
		m_extender->clock(INTERNAL_CLOCK_2MHZ, m_clockDivider);
	}
}

int EnginesWithEncoderMX1509SX1509::getClockDivider() {
	return m_clockDivider;
}

void EnginesWithEncoderMX1509SX1509::enableEnginesPins() {
	this->m_extender = new SX1509();
	m_extender->begin();
	m_extender->clock(INTERNAL_CLOCK_2MHZ, m_clockDivider);
	m_extender->pinMode(getLeftEnginePin1(), ANALOG_OUTPUT);
	m_extender->pinMode(getLeftEnginePin2(), ANALOG_OUTPUT);
	m_extender->pinMode(getRightEnginePin1(), ANALOG_OUTPUT);
	m_extender->pinMode(getRightEnginePin2(), ANALOG_OUTPUT);
	setIdlePower(255);
}

void EnginesWithEncoderMX1509SX1509::stopLeftEngine() {
	m_extender->analogWrite(getLeftEnginePin1(), getIdlePower());
	m_extender->analogWrite(getLeftEnginePin2(), getIdlePower());
}

void EnginesWithEncoderMX1509SX1509::stopRightEngine() {
	m_extender->analogWrite(getRightEnginePin1(), getIdlePower());
	m_extender->analogWrite(getRightEnginePin2(), getIdlePower());
}

void EnginesWithEncoderMX1509SX1509::go(int t_leftSpeed, int t_rightSpeed) {
	if (t_leftSpeed == 0 && t_rightSpeed == 0) {
		m_extender->analogWrite(getLeftEnginePin1(), getIdlePower());
		m_extender->analogWrite(getLeftEnginePin2(), getIdlePower());
		m_extender->analogWrite(getRightEnginePin1(), getIdlePower());
		m_extender->analogWrite(getRightEnginePin2(), getIdlePower());
		return;
	}
	if (t_leftSpeed > 0) {
		m_extender->analogWrite(getLeftEnginePin1(), t_leftSpeed);
		m_extender->analogWrite(getLeftEnginePin2(), getIdlePower());
	} else {
		m_extender->analogWrite(getLeftEnginePin1(), getIdlePower());
		m_extender->analogWrite(getLeftEnginePin2(), -t_leftSpeed);
	}

	if (t_rightSpeed > 0) {
		m_extender->analogWrite(getRightEnginePin1(), t_rightSpeed);
		m_extender->analogWrite(getRightEnginePin2(), getIdlePower());
	} else {
		m_extender->analogWrite(getRightEnginePin1(), getIdlePower());
		m_extender->analogWrite(getRightEnginePin2(), -t_rightSpeed);
	}
}
