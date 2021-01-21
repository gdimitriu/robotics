/*
 Engine driver with MX1509 and extender PCA9685
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

#include "EnginesWithEncoderMX1509PCA9685.h"

EnginesWithEncoderMX1509PCA9685::EnginesWithEncoderMX1509PCA9685(
		Adafruit_PWMServoDriver *t_pPwm, float t_whellRadius,
		long t_resolutionCoderLeft, long t_resolutionCoderRight,
		long t_countRotate1Inner, long t_countRotate1Outer,
		unsigned int t_leftEncoderPin, unsigned int t_rightEncoderPin) :
		EnginesWithEncoderMX1509(t_whellRadius, t_resolutionCoderLeft,
				t_resolutionCoderRight, t_countRotate1Inner,
				t_countRotate1Outer, t_leftEncoderPin, t_rightEncoderPin) {
	m_pPWMExpander = t_pPwm;
	setIdlePower(0);
}

EnginesWithEncoderMX1509PCA9685::~EnginesWithEncoderMX1509PCA9685() {
	// TODO Auto-generated destructor stub
}

void EnginesWithEncoderMX1509PCA9685::enableEnginesPins() {

}

void EnginesWithEncoderMX1509PCA9685::go(int t_leftSpeed, int t_rightSpeed) {
	if (t_leftSpeed == 0 && t_rightSpeed == 0) {
			m_pPWMExpander->setPWM(getLeftEnginePin1(), 0, getIdlePower());
			m_pPWMExpander->setPWM(getLeftEnginePin2(), 0, getIdlePower());
			m_pPWMExpander->setPWM(getRightEnginePin1(), 0, getIdlePower());
			m_pPWMExpander->setPWM(getRightEnginePin2(), 0, getIdlePower());
			return;
		}
		if (t_leftSpeed > 0) {
			m_pPWMExpander->setPWM(getLeftEnginePin1(), 0, t_leftSpeed);
			m_pPWMExpander->setPWM(getLeftEnginePin2(), 0, getIdlePower());
		} else {
			m_pPWMExpander->setPWM(getLeftEnginePin1(), 0, getIdlePower());
			m_pPWMExpander->setPWM(getLeftEnginePin2(), 0, -t_leftSpeed);
		}

		if (t_rightSpeed > 0) {
			m_pPWMExpander->setPWM(getRightEnginePin1(), 0, t_rightSpeed);
			m_pPWMExpander->setPWM(getRightEnginePin2(), 0, getIdlePower());
		} else {
			m_pPWMExpander->setPWM(getRightEnginePin1(), 0, getIdlePower());
			m_pPWMExpander->setPWM(getRightEnginePin2(), 0, -t_rightSpeed);
		}
}

void EnginesWithEncoderMX1509PCA9685::stopLeftEngine() {
	m_pPWMExpander->setPWM(getLeftEnginePin1(), 0, getIdlePower());
	m_pPWMExpander->setPWM(getLeftEnginePin2(), 0, getIdlePower());
}

void EnginesWithEncoderMX1509PCA9685::stopRightEngine() {
	m_pPWMExpander->setPWM(getRightEnginePin1(), 0, getIdlePower());
	m_pPWMExpander->setPWM(getRightEnginePin2(), 0, getIdlePower());
}
