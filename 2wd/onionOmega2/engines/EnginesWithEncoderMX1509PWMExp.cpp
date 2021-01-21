/*
 Engine driver with MX1509 and Onion PWM extender
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

#include "EnginesWithEncoderMX1509PWMExp.h"
#include <pwm-exp.h>

EnginesWithEncoderMX1509PWMExp::EnginesWithEncoderMX1509PWMExp(float t_whellRadius,
		long t_resolutionCoderLeft, long t_resolutionCoderRight,
		long t_countRotate1Inner, long t_countRotate1Outer,
		unsigned int t_leftEncoderPin, unsigned int t_rightEncoderPin) :
		EnginesWithEncoderMX1509(t_whellRadius, t_resolutionCoderLeft,
				t_resolutionCoderRight, t_countRotate1Inner,
				t_countRotate1Outer, t_leftEncoderPin, t_rightEncoderPin) {
	setIdlePower(0);
	setLeftEnginePower(100);
	setRightEnginePower(100);
	int status  = pwmDriverInit();
	if (status != 0) {
		printf("PWM expander not initialized\n");
		exit(-1);
	}
}

EnginesWithEncoderMX1509PWMExp::~EnginesWithEncoderMX1509PWMExp() {
	pwmDisableChip ();
}

void EnginesWithEncoderMX1509PWMExp::enableEnginesPins() {

}

void EnginesWithEncoderMX1509PWMExp::go(int t_leftSpeed, int t_rightSpeed) {
	pwmSetFrequency(50.0);
	if (t_leftSpeed == 0 && t_rightSpeed == 0) {
			pwmSetupDriver(getLeftEnginePin1(), getIdlePower(), 0);
			pwmSetupDriver(getLeftEnginePin2(), getIdlePower(), 0);
			pwmSetupDriver(getRightEnginePin1(),getIdlePower(), 0);
			pwmSetupDriver(getRightEnginePin2(),getIdlePower(), 0);
			return;
		}
		if (t_leftSpeed > 0) {
			pwmSetupDriver(getLeftEnginePin1(), t_leftSpeed, 0);
			pwmSetupDriver(getLeftEnginePin2(), getIdlePower(), 0);
		} else {
			pwmSetupDriver(getLeftEnginePin1(), getIdlePower(), 0);
			pwmSetupDriver(getLeftEnginePin2(), -t_leftSpeed, 0);
		}

		if (t_rightSpeed > 0) {
			pwmSetupDriver(getRightEnginePin1(), t_rightSpeed, 0);
			pwmSetupDriver(getRightEnginePin2(), getIdlePower(), 0);
		} else {
			pwmSetupDriver(getRightEnginePin1(), getIdlePower(), 0);
			pwmSetupDriver(getRightEnginePin2(), -t_rightSpeed, 0);
		}
}

void EnginesWithEncoderMX1509PWMExp::stopLeftEngine() {
	pwmSetFrequency(50.0);
	pwmSetupDriver(getLeftEnginePin1(), 100, 0);
	pwmSetupDriver(getLeftEnginePin2(), 100, 0);
}

void EnginesWithEncoderMX1509PWMExp::stopRightEngine() {
	pwmSetFrequency(50.0);
	pwmSetupDriver(getRightEnginePin1(), 100, 0);
	pwmSetupDriver(getRightEnginePin2(), 100, 0);
}
