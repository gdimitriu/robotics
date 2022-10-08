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
#ifndef EnginesSubSystem_h
#define EnginesSubSystem_h
#include <Arduino.h>
#include "AutonomousDroidSettings.h"
#include "SensorsSubSystem.h"

class EnginesSubSystem
{
public:
	EnginesSubSystem();
	void setMaximumMotorPowerLevel(int power);
	int getMaximumMotorPowerLevel();
	void setDebugMode(bool debug);
	void setSettings(AutonomousDroidSettings *settings);
	void setSensorsSubSystem(SensorsSubSystem *sensors);
	/*
	Set the pin for the motors.
	*/
	virtual void initialize() = 0;
	/*------------------------------------------------------
		Movement of the droid.
		This depend of how driver control the engines.
	-------------------------------------------------------*/
	/*
	power the motors to go in diffrent directions.
	*/
	virtual void go(int speedLeft, int speedRight) = 0;
	/*
	droid goes in 90 degree left.
	*/
	virtual void go90Left();
	/*
	droid goes in 90 degree right.
	*/
	virtual void go90Right();
	/*
	droid goes slow forward.
	*/
	virtual void goSlowForward();
	/*
	droid goes slow backward 10 times slower than forward.
	*/
	virtual void goSlowBackward();
	/*
	droid turn nr of degree 
	*/
	virtual void turnNrDegree(long degree);
protected:
	bool debugMode = false;
	int maximumPowerLevel = 255;
	AutonomousDroidSettings *droidSettings;
	SensorsSubSystem *droidSensors;

	virtual void turnNrDegreeLeft(unsigned long degree);
	virtual void turnNrDegreeRight(unsigned long degree);
};

#endif
