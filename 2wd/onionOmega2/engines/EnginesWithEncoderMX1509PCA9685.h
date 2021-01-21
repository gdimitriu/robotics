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

#ifndef ENGINES_ENGINESWITHENCODERMX1509PCA9685_H_
#define ENGINES_ENGINESWITHENCODERMX1509PCA9685_H_

#include <Adafruit_PWMServoDriver.h>
#include "EnginesWithEncoderMX1509.h"

class EnginesWithEncoderMX1509PCA9685: public EnginesWithEncoderMX1509 {
public:
	EnginesWithEncoderMX1509PCA9685(Adafruit_PWMServoDriver *t_pPwm,
			float t_whellRadius, long t_resolutionCoderLeft,
			long t_resolutionCoderRight, long t_countRotate1Inner,
			long t_countRotate1Outer, unsigned int t_leftEncoderPin,
			unsigned int t_rightEncoderPin);
	EnginesWithEncoderMX1509PCA9685();
	virtual ~EnginesWithEncoderMX1509PCA9685();
	virtual void enableEnginesPins();
	virtual void go(int t_leftSpeed, int t_rightSpeed);
	virtual void stopLeftEngine();
	virtual void stopRightEngine();
private:
	Adafruit_PWMServoDriver *m_pPWMExpander;
};

#endif /* ENGINES_ENGINESWITHENCODERMX1509PCA9685_H_ */
