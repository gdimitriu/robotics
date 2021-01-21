/*
 Calibrations with MX1509 and Onion PWM Expansion
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

#include <EnginesWithEncoderMX1509PWMExp.h>

#ifndef MICRO_CALIBRATIONS_CALIBRATIONMX1509PWMEXP_H_
#define MICRO_CALIBRATIONS_CALIBRATIONMX1509PWMEXP_H_

class CalibrationMX1509PWMExp {
public:
	CalibrationMX1509PWMExp();
	virtual ~CalibrationMX1509PWMExp();
	void start();
private:
	void printMenu();
	EnginesWithEncoderMX1509PWMExp * m_engines;
};

#endif /* MICRO_CALIBRATIONS_CALIBRATIONMX1509PCA9685_H_ */
