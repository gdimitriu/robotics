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
#ifndef EnginesSubSystemL298_h
#define EnginesSubSystemL298_h

#include "EnginesSubSystem.h"

class EnginesSubSystemL298 : public EnginesSubSystem
{
public:
	void addLeftMotorSettings(int firstPin, int secondPin, int thirdPin);
	void addRightMotorSettings(int firstPin, int secondPin, int thirdPin);
	/*
	Set the pin for the motors.
	*/
	virtual void initialize();
	/*------------------------------------------------------
		Movement of the droid.
		This depend of how driver control the engines.
	-------------------------------------------------------*/
	/*
	power the motors to go in diffrent directions.
	*/
	virtual void go(int speedLeft, int speedRight);
private:
	int leftMotorPin1 = -1;
	int leftMotorPin2 = -1;
	int leftMotorSpeedPin = -1;
	int rightMotorPin1 = -1;
	int rightMotorPin2 = -1;
	int rightMotorSpeedPin = -1;
};
#endif