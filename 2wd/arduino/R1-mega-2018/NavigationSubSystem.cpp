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

#include "NavigationSubSystem.h"
char NavigationSubSystem::getArgumentType()
{
	return 'f';
}

void NavigationSubSystem::setDebugMode(bool debug)
{
	this->debugMode = debug;
}

void NavigationSubSystem::setSensorsSubSystem(SensorsSubSystem *sensors)
{
	this->sensorsSubSystem = sensors;
}

void NavigationSubSystem::setSettings(AutonomousDroidSettings *settings)
{
	this->droidSettings = settings;
}

void NavigationSubSystem::setI2CBus(I2CSubSystem *i2c)
{
	this->i2cSubSystem = i2c;
}

void NavigationSubSystem::setDestinationLatitude(float latitude)
{
	this->destinationLatitude = latitude;
}

float NavigationSubSystem::getDestinationLatitude()
{
	return this->destinationLatitude;
}

void NavigationSubSystem::setDestinationLongitude(float longitude)
{
	this->destinationLongitude = longitude;
}

float NavigationSubSystem::getDestinationLongitude()
{
	return this->destinationLongitude;
}

void NavigationSubSystem::setMazeMode(bool mode)
{
	this->isMazeMode = mode;
}

float NavigationSubSystem::getKp()
{
	return this->Kp;
}

void NavigationSubSystem::setKp(float value)
{
	this->Kp = value;
}

float NavigationSubSystem::getKi()
{
	return this->Ki;
}

void NavigationSubSystem::setKi(float value)
{
	this->Ki = value;
}

float NavigationSubSystem::getKd()
{
	return this->Kd;
}

void NavigationSubSystem::setKd(float value)
{
	this->Kd = value;
}

float NavigationSubSystem::getCourseToDestination()
{
	return this->courseToDestination;
}

void NavigationSubSystem::setCourseToDestination(float destination)
{
	this->courseToDestination = destination;
}

CommandExecutionList < NavigationSubSystem, void (NavigationSubSystem::*)(float), float (NavigationSubSystem::*)(void)> * NavigationSubSystem::getCommandList()
{
	CommandExecutionList < NavigationSubSystem, void (NavigationSubSystem::*)(float), float (NavigationSubSystem::*)(void)> * commands =
		new CommandExecutionList < NavigationSubSystem, void (NavigationSubSystem::*)(float), float (NavigationSubSystem::*)(void)>();
	commands->add(new string("D001"), this, &NavigationSubSystem::setDestinationLatitude, &NavigationSubSystem::getDestinationLatitude);
	commands->add(new string("D002"), this, &NavigationSubSystem::setDestinationLongitude, &NavigationSubSystem::getDestinationLongitude);
	commands->add(new string("DC00"), this, &NavigationSubSystem::setCourseToDestination, &NavigationSubSystem::getCourseToDestination);
	commands->add(new string("DC01"), this, &NavigationSubSystem::setDirection, &NavigationSubSystem::getDirection);
	commands->add(new string("DA01"), this, &NavigationSubSystem::setKp, &NavigationSubSystem::getKp);
	commands->add(new string("DA02"), this, &NavigationSubSystem::setKi, &NavigationSubSystem::getKi);
	commands->add(new string("DA03"), this, &NavigationSubSystem::setKd, &NavigationSubSystem::getKd);
	return commands;
}

/*
This will take decision based on the great distance between left, right and front.
This will return:
0 for forward
-90 for left
90 for right
180 for backward
*/
float NavigationSubSystem::newDirection()
{
	bool frontLeftSensor = false;
	bool frontRightSensor = false;
	bool frontCentralSensor = false;
	unsigned long leftDistance;
	unsigned long rightDistance;
	sensorsSubSystem->readFrontSensors(frontLeftSensor,  frontCentralSensor, frontRightSensor);
	if (frontLeftSensor && frontRightSensor  && frontCentralSensor) {
		if (debugMode) {
			Serial.println("move back");
		}
		return 180;
	}
	unsigned long distance = sensorsSubSystem->calculateDistanceSweepSensor();
	if (debugMode) {
		Serial.print(distance); Serial.print(":"); Serial.print(droidSettings->getMinDistance()); Serial.print(":"); Serial.println(distance > droidSettings->getMinDistance());
	}
	if (distance > droidSettings->getMinDistance() && !frontLeftSensor && !frontRightSensor && !frontCentralSensor) {
		if (debugMode) {
			Serial.println("move forward");
		}
		return 0;
	}


	sensorsSubSystem->getLeftRightDistances(leftDistance, rightDistance);
	if (leftDistance == rightDistance) {
		if (debugMode) {
			Serial.println("move back");
		}
		return 180;
	}
	if (leftDistance < rightDistance && rightDistance >= droidSettings->getMinDistance() && !frontRightSensor) {
		if (debugMode) {
			Serial.println("move right");
		}
		return 90;
	}
	if (leftDistance > rightDistance && leftDistance >= droidSettings->getMinDistance() && !frontLeftSensor) {
		if (debugMode) {
			Serial.println("move left");
		}
		return -90;
	}
}

float NavigationSubSystem::newDirectionMaze()
{
	int direction = 0;
	if (!inMaze) {
		direction = newDirection();
		inMaze = true;
		if (direction == -90) {
			mazeSide = 180;
		}
		else if (direction == 90) {
			mazeSide = 0;
		}
		else {
			inMaze = false;
		}
		return direction;
	}
	bool frontLeftSensor = false;
	bool frontRightSensor = false;
	bool frontCentralSensor = false;
	sensorsSubSystem->readFrontSensors(frontLeftSensor, frontCentralSensor, frontRightSensor);
	if (frontLeftSensor && frontRightSensor && frontCentralSensor) {
		if (debugMode) {
			Serial.println("move back");
		}
		direction = 180;
	}
	unsigned long distance = sensorsSubSystem->calculateDistanceSweepSensor();
	if (debugMode) {
		Serial.print("Maze distance on left :");
		Serial.println(distance);
	}
	//we are parallel and blocked in front ... rotate right
	if (direction == 180 && abs(distance - droidSettings->getMinDistance()) < droidSettings->getErrorDistanceMaze()) {
		sensorsSubSystem->setServoPosition(mazeSide);
		distance = sensorsSubSystem->look(0);
		if (distance > droidSettings->getMinDistance()) {
			return 90;
		}
		else {
			//reset the maze operations go backward
			inMaze = false;
			return 180;
		}
	}
	//we are parallel and in front is free
	if (direction == 0 && abs(distance - droidSettings->getMinDistance()) < droidSettings->getErrorDistanceMaze()) {
		sensorsSubSystem->setServoPosition(180);
		distance = sensorsSubSystem->look(90);
		if (distance > droidSettings->getMinDistance()) {
			return 0;
		}
	}
}

float NavigationSubSystem::getDirection()
{
	proportionalComponent = courseToDestination - i2cSubSystem->getHeading();
	derivativeComponent = (proportionalComponent - lastError)/2;
	integrationComponent += proportionalComponent;
	float direction = proportionalComponent * Kp + integrationComponent * Ki + derivativeComponent * Kd;
	unsigned long distance = sensorsSubSystem->look(direction);
	if (distance >= droidSettings->getMinDistance()) {
		return direction;
	}
	if (!isMazeMode) {
		this->currentDirection =  newDirection();
	}
	else {
		this->currentDirection =  newDirectionMaze();
	}
	return this->currentDirection;
}

void  NavigationSubSystem::setDirection(float newDirection)
{
	this->currentDirection = newDirection;
}
