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
#include "SweepSensorHCSR04.h"

SweepSensorHCSR04::SweepSensorHCSR04(int trigPin, int echoPin)
{
	SENSOR_SWEEP_ECHO_PIN = echoPin;
	SENSOR_SWEEP_TRIG_PIN = trigPin;
	pinMode(SENSOR_SWEEP_ECHO_PIN, INPUT);
	pinMode(SENSOR_SWEEP_TRIG_PIN, OUTPUT);
}

unsigned long SweepSensorHCSR04::calculateDistance()
{
	return calculateDistance(SENSOR_SWEEP_TRIG_PIN, SENSOR_SWEEP_ECHO_PIN);
}

/*
* calculate the distance in mm using HC-SR04
*/
unsigned long SweepSensorHCSR04::calculateDistance(int trigPin, int echoPin)
{
	//set the trigPin to LOW
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	// Sets the trigPin on HIGH state for 10 micro seconds 
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(50);
	digitalWrite(trigPin, LOW);
	// Reads the echoPin, returns the sound wave travel time in microseconds 
	// Calculating the distance 
	unsigned long Time_Echo_us = pulseIn(echoPin, HIGH);  //calculate the pulse width at EchoPin, 
	if ((Time_Echo_us < 60000) && (Time_Echo_us > 1))     //a valid pulse width should be between (1, 60000).
	{
		return (Time_Echo_us * 34 / 100) / 2;
	}
	else {
		return MAX_DISTANCE;
	}
}