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

#include"AutonomousDroidSettings.h"
char AutonomousDroidSettings::getArgumentType()
{
	return 'l';
}

AutonomousDroidSettings::AutonomousDroidSettings()
{
	this->observers = new ObserverList();
}

void AutonomousDroidSettings::setObserver(Observer *observer)
{
	this->observers->addObserver(observer);
}

void AutonomousDroidSettings::setMaxDistance(unsigned long value)
{
	maxDistance = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setMinDistance(unsigned long value)
{	
	minDistance = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setErrorDistanceMaze(unsigned long value)
{
	errorDistanceMaze = value;
	observers->updateObservers();
}

unsigned long AutonomousDroidSettings::getMaxDistance()
{
	return maxDistance;
}

unsigned long AutonomousDroidSettings::getMinDistance()
{
	return minDistance;
}

unsigned long AutonomousDroidSettings::getErrorDistanceMaze()
{
	return errorDistanceMaze;
}


/*
* Timming setters.
*/
void AutonomousDroidSettings::setTiming90Left(unsigned long value)
{
	timing90Left = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setTiming90Right(unsigned long value)
{
	timing90Right = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setTimingSlowForward(unsigned long value)
{
	timingFrontSlow = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setTiming1Degree(unsigned long value)
{
	this->timing1Degree = value;
	this->timing90Left = 90 * value;
	this->timing90Right = 90 * value;
	observers->updateObservers();
}

/*
* Timming getters.
*/
unsigned long AutonomousDroidSettings::getTiming90Left()
{
	return timing90Left;
}

unsigned long AutonomousDroidSettings::getTiming90Right()
{
	return timing90Right;
}

unsigned long AutonomousDroidSettings::getTimingSlowForward()
{
	return timingFrontSlow;
}

unsigned long AutonomousDroidSettings::getTiming1Degree()
{
	return timing1Degree;
}

//setters for recharge in light values
void AutonomousDroidSettings::setMinRechargeLight(unsigned long value)
{
	this->minRechargeLight = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setMaxRechargeLight(unsigned long value)
{
	this->maxRechargeight = value;
	observers->updateObservers();
}

void AutonomousDroidSettings::setErrorRechargeLight(unsigned long value)
{
	this->errorRechargeLight = value;
	observers->updateObservers();
}

//getters for recharge in light values
unsigned long AutonomousDroidSettings::getMinRechargeLight()
{
	return this->minRechargeLight;
}

unsigned long AutonomousDroidSettings::getMaxRechargeLight()
{
	return this->maxRechargeight;
}

unsigned long AutonomousDroidSettings::getErrorRechargeLight()
{
	return this->errorRechargeLight;
}

//setters and getters for standby/sleep mode
void AutonomousDroidSettings::setSleepTimeout(unsigned long value)
{
	this->sleepTimeout = value;
}

unsigned long AutonomousDroidSettings::getSleepTimeout()
{
	return this->sleepTimeout;
}

CommandExecutionList < AutonomousDroidSettings, void (AutonomousDroidSettings::*)(unsigned long), unsigned long (AutonomousDroidSettings::*)(void)> *AutonomousDroidSettings::getCommandList()
{
	CommandExecutionList < AutonomousDroidSettings, void (AutonomousDroidSettings::*)(unsigned long), unsigned long (AutonomousDroidSettings::*)(void)> *commands =
		new CommandExecutionList < AutonomousDroidSettings, void (AutonomousDroidSettings::*)(unsigned long), unsigned long (AutonomousDroidSettings::*)(void)>();
	commands->add(new string("A000"), this, &AutonomousDroidSettings::setTiming1Degree, &AutonomousDroidSettings::getTiming1Degree);
	commands->add(new string("A001"), this, &AutonomousDroidSettings::setTiming90Left, &AutonomousDroidSettings::getTiming90Left);
	commands->add(new string("A002"), this, &AutonomousDroidSettings::setTiming90Right, &AutonomousDroidSettings::getTiming90Right);
	commands->add(new string("A003"), this, &AutonomousDroidSettings::setTimingSlowForward, &AutonomousDroidSettings::getTimingSlowForward);
	commands->add(new string("A101"), this, &AutonomousDroidSettings::setMaxDistance, &AutonomousDroidSettings::getMaxDistance);
	commands->add(new string("A102"), this, &AutonomousDroidSettings::setMinDistance, &AutonomousDroidSettings::getMinDistance);
	commands->add(new string("A103"), this, &AutonomousDroidSettings::setErrorDistanceMaze, &AutonomousDroidSettings::getErrorDistanceMaze);
	commands->add(new string("A201"), this, &AutonomousDroidSettings::setMinRechargeLight, &AutonomousDroidSettings::getMinRechargeLight);
	commands->add(new string("A202"), this, &AutonomousDroidSettings::setMaxRechargeLight, &AutonomousDroidSettings::getMaxRechargeLight);
	commands->add(new string("A203"), this, &AutonomousDroidSettings::setErrorRechargeLight, &AutonomousDroidSettings::getErrorRechargeLight);
	commands->add(new string("A301"), this, &AutonomousDroidSettings::setSleepTimeout, &AutonomousDroidSettings::getSleepTimeout);
	return commands;
}
