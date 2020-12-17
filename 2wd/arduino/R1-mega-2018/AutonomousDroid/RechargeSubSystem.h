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
#ifndef RechargeSubSystem_h
#define RechargeSubSystem_h
#include <avr/sleep.h>
#include <AutonomousDroidSettings.h>
#include <EnginesSubSystem.h>
#include <I2CSubSystem.h>
#include <NavigationSubSystem.h>

class RechargeSubSystem
{
public:
	RechargeSubSystem();
	void setSettings(AutonomousDroidSettings *settings);
	void setEnginesSubSystem(EnginesSubSystem *engines);
	void setNavigationSubSystem(NavigationSubSystem *navigation);
	void setI2CBus(I2CSubSystem *i2c);
	void addPhotoSensors(int leftPin, int rightPin);
	void setWakeUpSensor(int pin, int detectState);
	void setDebugMode(bool debug);
	void initialize();
	void sleepNow();
	int goToLight();
private:
	AutonomousDroidSettings * droidSettings = nullptr;
	//SubSystems
	EnginesSubSystem * enginesSubSystem = nullptr;
	NavigationSubSystem * navigationSubSystem = nullptr;
	I2CSubSystem *i2cBus = nullptr;
	//wakeUp system
	int pinWakeUpSensor = -1;
	int detectWakeUpSensor = 1;
	int stateForWakeUp = RISING;
	//light sensors
	int pinLeftLightSensor = -1;
	int pinRightLightSensor = -1;
	//light movement
	int HAS_MOVE_TO_LEFT = 100;
	int has_turn = 0;
	bool debugMode = false;
	char** bufferMessages;
};
#endif