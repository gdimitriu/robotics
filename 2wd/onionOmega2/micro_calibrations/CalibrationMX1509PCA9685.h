/*
 Calibrations with MX1509 and extender PCA9685
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

#include <EnginesWithEncoderMX1509PCA9685.h>

#ifndef MICRO_CALIBRATIONS_CALIBRATIONMX1509PCA9685_H_
#define MICRO_CALIBRATIONS_CALIBRATIONMX1509PCA9685_H_

class CalibrationMX1509PCA9685 {
public:
	CalibrationMX1509PCA9685();
	virtual ~CalibrationMX1509PCA9685();
	void start();
private:
	void printMenu();
	EnginesWithEncoderMX1509PCA9685 * m_engines;
	Adafruit_PWMServoDriver *m_pwmExpander;
};

#endif /* MICRO_CALIBRATIONS_CALIBRATIONMX1509PCA9685_H_ */
