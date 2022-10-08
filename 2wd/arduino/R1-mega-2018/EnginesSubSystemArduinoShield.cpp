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

#include "EnginesSubSystemArduinoShield.h"

void EnginesSubSystemArduinoShield::addLeftMotorSettings(int firstPin, int secondPin)
{
	leftMotorPin1 = firstPin;
	leftMotorPin2 = secondPin;
}

void EnginesSubSystemArduinoShield::addRightMotorSettings(int firstPin, int secondPin)
{
	rightMotorPin1 = firstPin;
	rightMotorPin2 = secondPin;
}

void EnginesSubSystemArduinoShield::initialize()
{
	pinMode(leftMotorPin1, OUTPUT);
	pinMode(leftMotorPin2, OUTPUT);
	pinMode(rightMotorPin1, OUTPUT);
	pinMode(rightMotorPin2, OUTPUT);
}

/*
* Move the droid in predefined directions.
*/
void EnginesSubSystemArduinoShield::go(int speedLeft, int speedRight)
{
	if (speedLeft == 0 && speedRight == 0) {
		analogWrite(leftMotorPin1, 0);
		analogWrite(leftMotorPin2, 0);
		analogWrite(rightMotorPin1, 0);
		analogWrite(rightMotorPin2, 0);
		return;
	}
	if (speedLeft > 0) {
		analogWrite(leftMotorPin1, speedLeft);
		analogWrite(leftMotorPin2, 0);
	}
	else {
		analogWrite(leftMotorPin1, 0);
		analogWrite(leftMotorPin2, -speedLeft);
	}

	if (speedRight > 0) {
		analogWrite(rightMotorPin1, speedRight);
		analogWrite(rightMotorPin2, 0);
	}
	else {
		analogWrite(rightMotorPin1, 0);
		analogWrite(rightMotorPin2, -speedRight);
	}
}
