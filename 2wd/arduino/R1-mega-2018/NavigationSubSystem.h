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
#ifndef NavigationSubSystem_h
#define NavigationSubSystem_h
#include "SensorsSubSystem.h"
#include "CommandExecutionList.h"
#include "I2CSubSystem.h"

class NavigationSubSystem
{
public:
	void setDebugMode(bool debug);
	void setSettings(AutonomousDroidSettings *settings);
	void setSensorsSubSystem(SensorsSubSystem *sensors);
	void setI2CBus(I2CSubSystem *i2c);
	char getArgumentType();
	/*
	*  Movement of the droid as autonomous
	*/
	//maze or other movement
	void setMazeMode(bool mode);
	float getDirection();
	void setDirection(float newDirection);
	/*------------------------------------------------
	* Navigation
	------------------------------------------------*/
	void setDestinationLatitude(float latitude);
	float getDestinationLatitude();
	void setDestinationLongitude(float longitude);
	float getDestinationLongitude();
	float getCourseToDestination();
	void setCourseToDestination(float destination);
	float getKp();
	void setKp(float value);
	float getKi();
	void setKi(float value);
	float getKd();
	void setKd(float value);
	CommandExecutionList < NavigationSubSystem, void (NavigationSubSystem::*)(float), float (NavigationSubSystem::*)(void)> *getCommandList();
private:
	/*
	Decision is based on the maxim distance between left and right.
	This will return:
	0 for forward
	-90 for left
	90 for right
	180 for backward
	*/
	float newDirection();
	float newDirectionMaze();
	bool debugMode = false;
	SensorsSubSystem *sensorsSubSystem = nullptr;
	AutonomousDroidSettings *droidSettings = nullptr;
	I2CSubSystem *i2cSubSystem = nullptr;
	//--------------------------------------------
	bool isMazeMode = false;
	bool inMaze = false;
	int mazeSide = 180; //left or right hand
	//navigation
	float currentLatitude = 0.0f;
	float destinationLatitude = 0.0f; //D001
	float currentLongitude = 0.0f;
	float destinationLongitude = 0.0f; //D002
	float courseToDestination = 0.0f;  //DC00
	float currentDirection = 0.0f;     //DC01
	//PID constants
	float Kp = 0.0f;				  //DA01
	float Ki = 0.0f;				  //DA02
	float Kd = 0.0f;				  //DA03
	//PID variables
	float proportionalComponent = 0.0f;
	float integrationComponent = 0.0f;
	float derivativeComponent = 0.0f;
	float errorFloat = 0.001f;
	float lastError = 0.0f;
};

#endif
