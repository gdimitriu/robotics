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

#include "CalibrationServoPCA9685.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 3) {
		printf(
				"Please call with the address of the extender and the pin number of the servo\n");
		return -1;
	}
	char *last;
	if (gpioInitialise() < 0) {
			perror("gpio init failed");
			exit(1);
	} else {
		CalibrationServoPCA9685 *servo = new CalibrationServoPCA9685(
			strtol(argv[1], &last, 16), atoi(argv[2]));
		servo->start();
		gpioTerminate();
		exit(0);
	}
}

CalibrationServoPCA9685::~CalibrationServoPCA9685() {
	delete m_driver;
}

CalibrationServoPCA9685::CalibrationServoPCA9685(uint8_t t_address,
		uint8_t t_pinNumber) {

	m_address = t_address;
	m_pinNumber = t_pinNumber;
	m_servoFreq = 50.0;
	m_servoMin = 100;
	m_servoMax = 440;
	m_servoFront = 280;
	m_usMin = 600;
	m_usMax = 2500;
	m_oscFreq = 27000000;
	m_driver = new Adafruit_PWMServoDriver(m_address);
	m_driver->begin();
	resetConfiguration();
}

void CalibrationServoPCA9685::resetConfiguration() {
	m_driver->reset();
	m_driver->setOscillatorFrequency(m_oscFreq);
	m_driver->setPWMFreq(m_servoFreq);
}

void CalibrationServoPCA9685::printMenu() {
	printf("Calibration of servo using PCA9685 extender\n");
	printf("\to - oscillator frequency [oxxx]\n");
	printf("\ts - servo frequency [sxxx.xx]\n");
	printf("\tum - servo minimum microseconds [umxxx]\n");
	printf("\tuM - servo maximum microseconds [uMxxx]\n");
	printf("\tl - rotate 90 degree left [l<xxx>]\n");
	printf("\tr - rotate 90 degree right[r<xxx>]\n");
	printf("\tf - front position [f<xxx>]\n");
	printf("\td - rotate xxx degree [dxxx]\n");
	printf("\tv - input manual the value [vxxx]\n");
	printf("\tq  - quit test\n");
	fflush(stdout);
}

void CalibrationServoPCA9685::start() {
	char *last;
	int iValue;
	long lValue;
	float fValue;
	printMenu();
	std::string inputParameter;
	while (1) {
		std::getline(std::cin, inputParameter);
		if (!inputParameter.empty()) {
			switch (inputParameter.c_str()[0]) {
			case 'q':
				m_driver->reset();
				m_driver->sleep();
				return;
			case 'o':
				inputParameter.erase(0, 1);
				lValue = strtol(inputParameter.c_str(), &last, 10);
				if (lValue > 0) {
					printf("Setting the Oscillator frequency from %d to %d\n",
							m_oscFreq, lValue);
					fflush(stdout);
					m_oscFreq = lValue;
					resetConfiguration();
				} else {
					printf(
							"Oscillator frequency should be greater than 0 but it was %s\n",
							inputParameter.c_str());
					fflush(stdout);
					printMenu();
				}
				break;
			case 's':
				inputParameter.erase(0, 1);
				fValue = strtof(inputParameter.c_str(), &last);
				if (fValue > 0) {
					printf("Setting the pwm servo frequency from %f to %f\n",
							m_servoFreq, fValue);
					fflush(stdout);
					m_servoFreq = fValue;
					resetConfiguration();
				} else {
					printf(
							"Servo frequency should be greater than 0 but it was %s\n",
							inputParameter.c_str());
					fflush(stdout);
					printMenu();
				}
				break;
			case 'u':
				inputParameter.erase(0, 1);
				switch (inputParameter.c_str()[0]) {
				case 'm':
					inputParameter.erase(0, 1);
					iValue = strtol(inputParameter.c_str(), &last, 10);
					if (iValue > 0) {
						printf("Set the minimum microseconds from %d to %d\n",
								m_usMin, iValue);
						fflush(stdout);
						m_usMin = iValue;
					} else {
						printf(
								"Minimum value should be greater than 0 but it was %s",
								inputParameter.c_str());
						fflush(stdout);
						printMenu();
					}
					break;
				case 'M':
					inputParameter.erase(0, 1);
					iValue = strtol(inputParameter.c_str(), &last, 10);
					if (iValue > 0) {
						printf("Set the maximum microseconds from %d to %d\n",
								m_usMax, iValue);
						fflush(stdout);
						m_usMax = iValue;
					} else {
						printf(
								"Maximum value should be greater than 0 but it was %s",
								inputParameter.c_str());
						fflush(stdout);
						printMenu();
					}
					break;
				default:
					printf("Microsecond should be only m or M but was %s\n",
							inputParameter.c_str());
					fflush(stdout);
					printMenu();
				}
				break;
			case 'l':
				inputParameter.erase(0, 1);
				if (!inputParameter.empty()) {
					iValue = strtol(inputParameter.c_str(), &last, 10);
					if (iValue > 0) {
						printf("Set servo minimum from %d to %d\n", m_servoMin,
								iValue);
						fflush(stdout);
						m_servoMin = iValue;
					} else {
						printf(
								"Minimum value should be greater than 0 but it was %s",
								inputParameter.c_str());
						fflush(stdout);
						break;
					}
				}
				m_driver->setPWM(m_pinNumber, 0, m_servoMin);
				break;
			case 'r':
				inputParameter.erase(0, 1);
				if (!inputParameter.empty()) {
					iValue = strtol(inputParameter.c_str(), &last, 10);
					if (iValue > 0) {
						printf("Set servo maximum from %d to %d\n", m_servoMax,
								iValue);
						fflush(stdout);
						m_servoMax = iValue;
					} else {
						printf(
								"Maximum value should be greater than 0 but it was %s",
								inputParameter.c_str());
						fflush(stdout);
						printMenu();
						break;
					}
				}
				m_driver->setPWM(m_pinNumber, 0, m_servoMax);
				break;
			case 'f':
				inputParameter.erase(0, 1);
				if (!inputParameter.empty()) {
					iValue = strtol(inputParameter.c_str(), &last, 10);
					if (iValue > 0) {
						printf("Set servo front from %d to %d\n", m_servoFront,
								iValue);
						fflush(stdout);
						m_servoFront = iValue;
					} else {
						printf(
								"Servo front value should be greater than 0 but it was %s",
								inputParameter.c_str());
						fflush(stdout);
						printMenu();
						break;
					}
				}
				m_driver->setPWM(m_pinNumber, 0, m_servoFront);
				break;
			case 'v':
				inputParameter.erase(0, 1);
				iValue = strtol(inputParameter.c_str(), &last, 10);
				if (iValue > 0) {
					m_driver->setPWM(m_pinNumber, 0, iValue);
				} else {
					printf(
							"Movement should be positive between 0 and 180 but it was %s\n",
							inputParameter.c_str());
					fflush(stdout);
					printMenu();
				}
				break;
			case 'd':
				inputParameter.erase(0, 1);
				iValue = strtol(inputParameter.c_str(), &last, 10);
				if (iValue > 0) {
					m_driver->setPWM(m_pinNumber, 0,
							m_servoMin
									+ ((m_servoMax - m_servoMin) / 180) * iValue);
				} else {
					printf(
							"Rotation angle should be positive between 0 and 180 but it was %s\n",
							inputParameter.c_str());
					fflush(stdout);
					printMenu();
				}
				break;
			default:
				printMenu();
			}
		}
	}
}

