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

#include "EnginesSubSystemL298.h"

void EnginesSubSystemL298::addLeftMotorSettings(int firstPin, int secondPin, int thirdPin)
{
	leftMotorPin1 = firstPin;
	leftMotorPin2 = secondPin;
	leftMotorSpeedPin = thirdPin;
}

void EnginesSubSystemL298::addRightMotorSettings(int firstPin, int secondPin, int thirdPin)
{
	rightMotorPin1 = firstPin;
	rightMotorPin2 = secondPin;
	rightMotorSpeedPin = thirdPin;
}

void EnginesSubSystemL298::initialize()
{
	pinMode(leftMotorPin1, OUTPUT);
	pinMode(leftMotorPin2, OUTPUT);
	pinMode(leftMotorSpeedPin, OUTPUT);
	pinMode(rightMotorPin1, OUTPUT);
	pinMode(rightMotorPin2, OUTPUT);
	pinMode(rightMotorSpeedPin, OUTPUT);
}

/*
* Move the droid in predefined directions.
*/
void EnginesSubSystemL298::go(int speedLeft, int speedRight)
{
	if (speedLeft == 0 && speedRight == 0) {
		digitalWrite(leftMotorPin1, 0);
		digitalWrite(leftMotorPin2, 0);
		analogWrite(leftMotorSpeedPin, 0);
		digitalWrite(rightMotorPin1, 0);
		digitalWrite(rightMotorPin2, 0);
		analogWrite(rightMotorSpeedPin, 0);
		return;
	}
	if (speedLeft > 0) {
		digitalWrite(leftMotorPin1, 0);
		digitalWrite(leftMotorPin2, 1);
		analogWrite(leftMotorSpeedPin, speedLeft);
	}
	else {
		digitalWrite(leftMotorPin1, 1);
		digitalWrite(leftMotorPin2, 0);
		analogWrite(leftMotorSpeedPin, -speedLeft);
	}

	if (speedRight > 0) {
		digitalWrite(rightMotorPin1, 0);
		digitalWrite(rightMotorPin2, 1);
		analogWrite(rightMotorSpeedPin, speedRight);
	}
	else {
		digitalWrite(rightMotorPin1, 1);
		digitalWrite(rightMotorPin2, 0);
		analogWrite(rightMotorSpeedPin, -speedRight);
	}
}
