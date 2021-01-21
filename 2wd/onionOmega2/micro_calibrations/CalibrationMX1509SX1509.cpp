/*
 Calibrations with MX1509 and extender SX1509
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

#include "CalibrationMX1509SX1509.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

int main()
{
	CalibrationMX1509SX1509* droid = new CalibrationMX1509SX1509();
	droid->start();
	delete droid;
}

CalibrationMX1509SX1509::CalibrationMX1509SX1509() {
	//settings for micro see documentation from docs when it will be available
	m_engines = new EnginesWithEncoderMX1509SX1509(3, 1452, 1468, 9, 10, 3, 2);
	m_engines->setEnginesPins(0, 1, 2, 3);
	m_engines->setClockDivider(7);
	m_engines->enableEnginesPins();
}

CalibrationMX1509SX1509::~CalibrationMX1509SX1509() {
	delete m_engines;
}

void CalibrationMX1509SX1509::printMenu() {
	printf("Calibrations of SX1509 with MX1509 on onion Omega\n");
	printf("Print Encoders values (p)\n");
	printf("Clear Encoders values (c)\n");
	printf("Full stop (s)\n");
	printf("Linear move (-)xxx cm (l(-)xxx)\n");
	printf("Rotate xxx degree (- for left rotation) (r(-)xxx)\n");
	printf("Change engine power max value in range 0-255 (vxxx)\n");
	printf("Change engine power idle value in range 0-255 (i(xxx)\n");
	printf("Change clock divider value 1-64? (dxx)\n");
	printf("Quit (q)\n");
	fflush(stdout);
}

void CalibrationMX1509SX1509::start() {
	char *last;
	float fValue;
	long lValue;
	printMenu();
	std::string inputParameter;
	std::getline(std::cin, inputParameter);
	while (1) {
		if (!inputParameter.empty()) {
			switch (inputParameter.front()) {
			case 'q':
				m_engines->fullStop();
				return;
			case 's':
				printf("Full stop all m_engines!\n");
				fflush(stdout);
				m_engines->fullStop();
				break;
			case 'p':
				printf("Left Encoder=%d Right Encoder= %d\n",
						m_engines->getLeftCounter(), m_engines->getRightCounter());
				fflush(stdout);
				break;
			case 'c':
				printf("Clear Counters with actual values\n");
				printf("Left Encoder=%d Right Encoder=%d\n",
						m_engines->getLeftCounter(), m_engines->getRightCounter());
				fflush(stdout);
				m_engines->resetCounters();
				break;
			case 'l':
				inputParameter.erase(0, 1);
				if (inputParameter.size() == 1) {
					if (inputParameter.front() == '+') {
						printf("Moving forward until stop !!!\n");
						fflush(stdout);
						m_engines->go(m_engines->getLeftEnginePower(),
								m_engines->getRightEnginePower());
						break;
					} else if (inputParameter.front() == '-') {
						printf("Moving backward until stop !!!\n");
						fflush(stdout);
						m_engines->go(-m_engines->getLeftEnginePower(),
								-m_engines->getRightEnginePower());
						break;
					}
				}
				fValue = strtof(inputParameter.c_str(), &last);
				if (fValue != 0) {
					printf("Linear move %lf cm\n", fValue);
					fflush(stdout);
					m_engines->linearMove(fValue);
				} else {
					printf("Movement should be a double but it was : %s\n",
							inputParameter.c_str());
					printMenu();
				}
				break;
			case 'r':
				inputParameter.erase(0, 1);
				lValue = strtol(inputParameter.c_str(), &last, 10);
				if (lValue != 0) {
					printf("Rotate %ld degree\n", lValue);
					fflush(stdout);
					m_engines->rotateDegree((int) lValue);
				} else {
					printf("Rotation angle should be a long but it was : %s\n",
							inputParameter.c_str());
					printMenu();
					break;
				}
				break;
			case 'v':
				inputParameter.erase(0, 1);
				lValue = strtol(inputParameter.c_str(), &last, 10);
				if (lValue >= 0 && lValue < 256) {
					printf("Changed engine max power from %d : %d to %d\n",
							m_engines->getLeftEnginePower(),
							m_engines->getRightEnginePower(), lValue);
					fflush(stdout);
					m_engines->setLeftEnginePower((unsigned int) lValue);
					m_engines->setRightEnginePower((unsigned int) lValue);
				} else {
					printf(
							"Engine max power should be between 0 and 255 but it was : %s\n",
							inputParameter.c_str());
					printMenu();
				}
				break;
			case 'i':
				inputParameter.erase(0, 1);
				lValue = strtol(inputParameter.c_str(), &last, 10);
				if (lValue >= 0 && lValue < 256) {
					printf("Changed engine idle power from %d to %d\n",
							m_engines->getIdlePower(), lValue);
					fflush(stdout);
					m_engines->setIdlePower((unsigned int) lValue);
				} else {
					printf(
							"Engine idle power should be between 0 and 255 but it was : %s\n",
							inputParameter.c_str());
					printMenu();
				}
				break;
			case 'd':
				inputParameter.erase(0, 1);
				lValue = strtol(inputParameter.c_str(), &last, 10);
				if (lValue > 0 && lValue < 65) {
					printf("Changed clock divider for SX1509 from %d to %ld\n",
							m_engines->getClockDivider(), lValue);
					fflush(stdout);
					m_engines->setClockDivider((unsigned char) lValue);
				} else {
					printf(
							"Clock divider for SX1509 should be between 1 and 64 but was : %s\n",
							inputParameter.c_str());
					printMenu();
				}
				break;
			default:
				printMenu();
			}
		}
		std::getline(std::cin, inputParameter);
	}
}
