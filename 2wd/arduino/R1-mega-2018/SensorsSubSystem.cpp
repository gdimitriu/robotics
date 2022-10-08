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

#include "SensorsSubSystem.h"

SensorsSubSystem::SensorsSubSystem()
{

}
void SensorsSubSystem::setDroidSettings(AutonomousDroidSettings *settings)
{
	this->droidSettings = settings;
	this->droidSettings->setObserver(this);
}

void SensorsSubSystem::setDebugMode(bool debug)
{
	this->debugMode = debug;
}

void SensorsSubSystem::attachFrontLeftSensor(int sensorPin, int detectState)
{
	this->pinFrontLeftSensor = sensorPin;
	this->detectFrontLeftSensor = detectState;
}

void SensorsSubSystem::attachFrontCentralSensor(int sensorPin, int detectState)
{
	this->pinFrontCentralSensor = sensorPin;
	this->detectFrontCentralSensor = detectState;
}

void SensorsSubSystem::attachFrontRightSensor(int sensorPin, int detectState)
{
	this->pinFrontRightSensor = sensorPin;
	this->detectFrontRightSensor = detectState;
}

void SensorsSubSystem::attachFrontSweepSensor(int servoPin, SweepSensor *frontSensor)
{
	this->frontSweepSensor = frontSensor;
	sensorServo.attach(servoPin);	
}

void SensorsSubSystem::attachReverseSensor(int sensorPin, int detectState)
{
	this->pinReverseSensor = sensorPin;
	this->detectReverseSensor = detectState;
}

void SensorsSubSystem::initialize()
{
	pinMode(pinFrontLeftSensor, INPUT);
	pinMode(pinFrontCentralSensor, INPUT);
	pinMode(pinFrontRightSensor, INPUT);
	pinMode(pinReverseSensor, INPUT);
	//reset servo
	sensorServo.write(oldServoPosition);
	frontSweepSensor->setMaxDistance(droidSettings->getMaxDistance());
}

void SensorsSubSystem::update()
{
	frontSweepSensor->setMaxDistance(droidSettings->getMaxDistance());
}

void SensorsSubSystem::setServoPosition(int position)
{
	this->oldServoPosition = position;
}
/*-------------------------------------------------------------------------------
 * Getters for the reading values
 *-----------------------------------------------------------------------------*/
/*
Look on the desired position.
*/
unsigned long SensorsSubSystem::look(int position)
{
	sensorServo.write(position);
	//wait to position the servo
	delay(700);
	unsigned long distance = frontSweepSensor->calculateDistance();
	if (debugMode) {
		Serial.print("look at position:"); Serial.print(position);
		Serial.println(" has distance:"); Serial.println(distance);
	}
	sensorServo.write(oldServoPosition);
	//wait to position the servo
	delay(700);
	return distance;
}

void SensorsSubSystem::readFrontSensors(bool &frontLeftSensor, bool &frontCentralSensor, bool &frontRightSensor)
{
	if (pinFrontLeftSensor != -1 && pinFrontRightSensor != -1) {
		frontLeftSensor = (digitalRead(pinFrontLeftSensor) == detectFrontLeftSensor);
		frontRightSensor = (digitalRead(pinFrontRightSensor) == detectFrontRightSensor);
	}
	else {
		frontLeftSensor = false;
		frontRightSensor = false;
	}
	if (pinFrontCentralSensor != -1) {
		frontCentralSensor = (digitalRead(pinFrontCentralSensor) == detectFrontCentralSensor);
	}
	else {
		frontCentralSensor = false;
	}
}

bool SensorsSubSystem::hasFrontSensors()
{
	if (pinFrontLeftSensor != -1 && pinFrontRightSensor != -1) return true;
	return false;
}

/*
Get the left and right distances from sweep sensor.
return in first and second argument the distances
*/
void SensorsSubSystem::getLeftRightDistances(unsigned long &leftDistance, unsigned long &rightDistance)
{
	if (hasFrontSensors()) {
		if (isFrontLeftObstructed()) {
			//look right
			rightDistance = look(0);
			leftDistance = 0;
			return;
		}
		if (isFrontRightObstructed()) {
			//look left
			leftDistance = look(180);
			rightDistance = 0;
			return;
		}
	}
	//look right
	rightDistance = look(0);
	//look left
	leftDistance = look(180);
}

bool SensorsSubSystem::isFrontLeftObstructed()
{
	return detectFrontLeftSensor == digitalRead(pinFrontLeftSensor);
}

bool SensorsSubSystem::isFrontCentralObstructed()
{
	return detectFrontCentralSensor == digitalRead(pinFrontCentralSensor);
}

bool SensorsSubSystem::isFrontRightObstructed()
{
	return detectFrontRightSensor == digitalRead(pinFrontRightSensor);
}

bool SensorsSubSystem::isReverseObstructed()
{
	return detectReverseSensor == digitalRead(pinReverseSensor);
}

unsigned long SensorsSubSystem::calculateDistanceSweepSensor()
{
	if (frontSweepSensor != nullptr) {
		return frontSweepSensor->calculateDistance();
	}
	else {
		return droidSettings->getMaxDistance();
	}
}
