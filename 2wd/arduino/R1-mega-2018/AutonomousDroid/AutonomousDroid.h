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
#ifndef AutonomousDroid_h
#define AutonomousDroid_h
#include <Arduino.h>
#include <I2CSubSystem.h>
#include <CommandSubSystem.h>
#include <AutonomousDroidSettings.h>
#include <EnginesSubSystem.h>
#include <SensorsSubSystem.h>
#include <NavigationSubSystem.h>
#include <RechargeSubSystem.h>

class AutonomousDroid
{
public:
	AutonomousDroid();
	~AutonomousDroid();
	//setters
	void setSettings(AutonomousDroidSettings *settings);
	void setDebugMode(bool debug);
	void setMazeMode(bool mode);
	//subSystems
	void setEnginesSubSystem(EnginesSubSystem *engines);
	void setSensorsSubSystem(SensorsSubSystem *sensors);
	void setNavigationSubSystem(NavigationSubSystem *navigation);
	void setRechargeSubSystem(RechargeSubSystem *recharge);	
	void setI2CBus(I2CSubSystem *i2c);
	void setCommandSubSystem(CommandSubSystem *commandSubSystem);
	//setup the system
	bool setup();
	//move arbitrary or by command
	void move();
	void goToLight();
private:	
	bool isMazeMode = false;
	//--------------------------------------------
	bool debugMode = false;
	//--------------------------------------------
	/*
	 * Subsystems
	*/
	AutonomousDroidSettings *settings = nullptr;
	I2CSubSystem *i2cBus = nullptr;
	CommandSubSystem *commandSubSystem = nullptr;
	SensorsSubSystem *sensorsSubSystem = nullptr;
	EnginesSubSystem *enginesSubSystem = nullptr;
	NavigationSubSystem *navigationSubSystem = nullptr;
	RechargeSubSystem *rechargeSubSystem = nullptr;
};

#endif