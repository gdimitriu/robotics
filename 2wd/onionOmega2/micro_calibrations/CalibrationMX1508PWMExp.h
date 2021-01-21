/*
 Calibrations with MX1508 and Onion PWM Expansion
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

#include "EnginesWithEncoderMX1508PWMExp.h"

#ifndef MICRO_CALIBRATIONS_CALIBRATIONMX1508PWMEXP_H_
#define MICRO_CALIBRATIONS_CALIBRATIONMX1508PWMEXP_H_

class CalibrationMX1508PWMExp {
public:
	CalibrationMX1508PWMExp();
	virtual ~CalibrationMX1508PWMExp();
	void start();
private:
	void printMenu();
	EnginesWithEncoderMX1508PWMExp * m_engines;
};

#endif /* MICRO_CALIBRATIONS_CALIBRATIONMX1508PCA9685_H_ */
