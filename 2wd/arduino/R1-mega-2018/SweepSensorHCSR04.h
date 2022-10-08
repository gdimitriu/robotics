/*
 * Copyright 2018 Gabriel Dimitriu

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
#pragma once
#ifndef SweepSensorHCSR04_h
#define SweepSensorHCSR04_h

#include "SweepSensor.h"

class SweepSensorHCSR04 : public SweepSensor
{
public:
	SweepSensorHCSR04(int trigPin, int echoPin);

	virtual unsigned long calculateDistance();

private:

	/*
	* calculate the distance in mm using HC-SR04
	*/
	unsigned long calculateDistance(int trigPin, int echoPin);

	//pins for the HR-SR04 ultrasonic senzor
	int SENSOR_SWEEP_ECHO_PIN = 0;
	int SENSOR_SWEEP_TRIG_PIN = 0;
};
#endif