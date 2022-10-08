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
#include "EnginesSubSystem.h"

EnginesSubSystem::EnginesSubSystem()
{

}

void EnginesSubSystem::setMaximumMotorPowerLevel(int power)
{
	this->maximumPowerLevel = power;
}

int EnginesSubSystem::getMaximumMotorPowerLevel()
{
	return maximumPowerLevel;
}

void  EnginesSubSystem::setDebugMode(bool debug)
{
	this->debugMode = debug;
}

void EnginesSubSystem::setSettings(AutonomousDroidSettings *settings)
{
	this->droidSettings = settings;
}

void EnginesSubSystem::setSensorsSubSystem(SensorsSubSystem *sensors)
{
	this->droidSensors = sensors;
}

void EnginesSubSystem::goSlowForward()
{
	go(maximumPowerLevel, maximumPowerLevel);
	delay(droidSettings->getTimingSlowForward());
	go(0, 0);
}

void EnginesSubSystem::go90Left()
{
	//wheel sensors are disabled
	if (!droidSensors->hasFrontSensors()) {
		go(-maximumPowerLevel, maximumPowerLevel);
		delay(droidSettings->getTiming90Left());
		go(0, 0);
		return;
	}

	int step = droidSettings->getTiming90Right() / droidSettings->getTimingSlowForward();
	for (int i = 0; i < step; i++) {
		while (droidSensors->isFrontRightObstructed()) {
			goSlowBackward();
		}
		go(-maximumPowerLevel, maximumPowerLevel);
		delay(droidSettings->getTimingSlowForward());
		go(0, 0);
	}
}

void EnginesSubSystem::go90Right()
{
	//wheel sensors are disabled
	if (!droidSensors->hasFrontSensors()) {
		go(maximumPowerLevel, -maximumPowerLevel);
		delay(droidSettings->getTiming90Right());
		go(0, 0);
		return;
	}
	int step = droidSettings->getTiming90Right() / droidSettings->getTimingSlowForward();
	for (int i = 0; i < step; i++) {
		while (droidSensors->isFrontLeftObstructed()) {
			goSlowBackward();
		}

		go(maximumPowerLevel, -maximumPowerLevel);
		delay(droidSettings->getTimingSlowForward());
		go(0, 0);
	}
}

void EnginesSubSystem::turnNrDegree(long degree)
{
	if (degree < 0) {
		turnNrDegreeLeft(-degree);
	}
	else {
		turnNrDegreeRight(degree);
	}
}

void EnginesSubSystem::turnNrDegreeLeft(unsigned long degree)
{
	//front left/right sensors are disabled
	if (!droidSensors->hasFrontSensors()) {
		go(-maximumPowerLevel, maximumPowerLevel);
		for (long i = 0; i < degree; i++) {
			delay(droidSettings->getTiming1Degree());
		}
		go(0, 0);
		return;
	}
	for (long i = 0; i < degree; i++)
	{
		while (droidSensors->isFrontRightObstructed()) {
			goSlowBackward();
		}
		go(-maximumPowerLevel, maximumPowerLevel);
		delay(droidSettings->getTiming1Degree());
		go(0, 0);
	}
}

void EnginesSubSystem::turnNrDegreeRight(unsigned long degree)
{
	//front left/right sensors are disabled
	if (!droidSensors->hasFrontSensors()) {
		go(maximumPowerLevel, -maximumPowerLevel);
		for (long i = 0; i < degree; i++) {
			delay(droidSettings->getTiming1Degree());
		}
		go(0, 0);
		return;
	}
	for (long i = 0; i < degree; i++)
	{
		while (droidSensors->isFrontLeftObstructed()) {
			goSlowBackward();
		}
		go(maximumPowerLevel, -maximumPowerLevel);
		delay(droidSettings->getTiming1Degree());
		go(0, 0);
	}
}

void EnginesSubSystem::goSlowBackward()
{
	if (!droidSensors->hasFrontSensors()) {
		//new decision rotate
		if (droidSensors->isReverseObstructed()) {
			unsigned long leftDistance;
			unsigned long rightDistance;
			droidSensors->getLeftRightDistances(leftDistance, rightDistance);
			if (leftDistance < rightDistance) {
				go90Right();
			}
			else {
				go90Left();
			}
		}
		go(-maximumPowerLevel, -maximumPowerLevel);
		delay(droidSettings->getTimingSlowForward() / 2);
		go(0, 0);
		return;
	}
	if (debugMode) {
		Serial.println("go slow backward");
	}
	int step = droidSettings->getTimingSlowForward() / 20;
	for (int i = 0; i < step; i++) {
		go(-maximumPowerLevel, -maximumPowerLevel);
		delay(droidSettings->getTimingSlowForward() / 20);
		go(0, 0);

		while (droidSensors->isReverseObstructed()) {
			bool leftObstructed = droidSensors->isFrontLeftObstructed();
			bool rightObstructed = droidSensors->isFrontRightObstructed();
			if (leftObstructed && rightObstructed) {
				go(-maximumPowerLevel, -maximumPowerLevel);
				delay(droidSettings->getTimingSlowForward() / 20);
				go(0, 0);
			}
			else if (leftObstructed) {
				go(-maximumPowerLevel, maximumPowerLevel);
				delay(droidSettings->getTimingSlowForward() / 20);
				go(0, 0);
			}
			else if (rightObstructed) {
				go(maximumPowerLevel, -maximumPowerLevel);
				delay(droidSettings->getTimingSlowForward() / 20);
				go(0, 0);
			}
		}
	}
}
