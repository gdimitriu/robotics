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
#ifndef AutonomousDroidSettings_h
#define AutonomousDroidSettings_h
#include <Observer.h>
#include <ObserverList.h>
#include <CommandExecutionList.h>

class AutonomousDroidSettings
{
public:
	AutonomousDroidSettings();
	char getArgumentType();
	//setters for distance
	void setMaxDistance(unsigned long maxDistance);
	void setMinDistance(unsigned long minDistance);
	void setErrorDistanceMaze(unsigned long errorDistance);
	//getters for distance
	unsigned long getMaxDistance();
	unsigned long getMinDistance();
	unsigned long getErrorDistanceMaze();
	//setters for timing (depend on the moving platform and weight)
	void setTiming90Left(unsigned long value);
	void setTiming90Right(unsigned long value);
	void setTimingSlowForward(unsigned long value);
	void setTiming1Degree(unsigned long value);
	//getters for timing (depend on the moving platform and weight)
	unsigned long getTiming90Left();
	unsigned long getTiming90Right();
	unsigned long getTimingSlowForward();
	unsigned long getTiming1Degree();
	//setters for recharge in light values
	void setMinRechargeLight(unsigned long value);
	void setMaxRechargeLight(unsigned long value);
	void setErrorRechargeLight(unsigned long value);
	//getters for recharge in light values
	unsigned long getMinRechargeLight();
	unsigned long getMaxRechargeLight();
	unsigned long getErrorRechargeLight();
	//sleep-power down mode
	void setSleepTimeout(unsigned long value);
	unsigned long getSleepTimeout();
	//observers to be notified when configuration is changed
	void setObserver(Observer *observer);
	//get the list of commands
	CommandExecutionList < AutonomousDroidSettings, void (AutonomousDroidSettings::*)(unsigned long), unsigned long (AutonomousDroidSettings::*)(void)> *getCommandList();
private:
	ObserverList *observers = nullptr;
	/*
	* Moving decision constants.
	*/
	//timing to go
	unsigned long timing1Degree = 0L;	//A000
	unsigned long timing90Left = 0L;    //A001
	unsigned long timing90Right = 0L;   //A002
	unsigned long timingFrontSlow = 0L; //A003
	//distance
	unsigned long maxDistance = 0L;       //A101
	unsigned long minDistance = 0L;       //A102
	unsigned long errorDistanceMaze = 0L; //A103
	/*
	* Recharge decision constants
	*/
	unsigned long minRechargeLight = 0L;   //A201
	unsigned long maxRechargeight = 0L;    //A202
	unsigned long errorRechargeLight = 0L; //A203
	/*standby mode*/
	unsigned long sleepTimeout = 0L;
};

#endif