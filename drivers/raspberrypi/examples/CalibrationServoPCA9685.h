/*
 * Calibrations with sg9 servo and extender PCA9685
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
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

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

#ifndef ADAFRUIT_PWMSERVODRIVER_EXAMPLES_CALIBRATIONSERVOPCA9685_H_
#define ADAFRUIT_PWMSERVODRIVER_EXAMPLES_CALIBRATIONSERVOPCA9685_H_


class CalibrationServoPCA9685 {
public:
	CalibrationServoPCA9685(uint8_t t_address, uint8_t t_pinNumber);
	virtual ~CalibrationServoPCA9685();
	void start();
private:
	void printMenu();
	void resetConfiguration();
	uint8_t m_address;
	uint8_t m_pinNumber;
	Adafruit_PWMServoDriver *m_driver;
	uint16_t m_servoMin;
	uint16_t m_servoMax;
	uint16_t m_servoFront;
	uint16_t m_usMin;
	uint16_t m_usMax;
	float m_servoFreq;
	uint32_t m_oscFreq;
};

#endif /* ADAFRUIT_PWMSERVODRIVER_EXAMPLES_CALIBRATIONSERVOPCA9685_H_ */
