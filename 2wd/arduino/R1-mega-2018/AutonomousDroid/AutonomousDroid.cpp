/*
 * Copyright 2018 Gabriel Dimitriu
 * 
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

#include <AutonomousDroid.h>

AutonomousDroid::AutonomousDroid()
{
}

AutonomousDroid::~AutonomousDroid()
{
}

void AutonomousDroid::setDebugMode(bool debug)
{
	debugMode = debug;
}

void AutonomousDroid::setI2CBus(I2CSubSystem *i2c)
{
	i2cBus = i2c;
}

bool AutonomousDroid::setup()
{
	if(i2cBus != nullptr) i2cBus->initialize();
	if (sensorsSubSystem == nullptr) return false;
	sensorsSubSystem->initialize();
	if (enginesSubSystem == nullptr) return false;	
	enginesSubSystem->initialize();
	return true;
	if (rechargeSubSystem != nullptr) rechargeSubSystem->initialize();
}

void AutonomousDroid::setSettings(AutonomousDroidSettings *settings)
{
	this->settings = settings;
}

void AutonomousDroid::setCommandSubSystem(CommandSubSystem *commandSubSystem)
{
	this->commandSubSystem = commandSubSystem;
	if (this->settings != nullptr) {
		this->commandSubSystem->setAutonomousDroidSettings(this->settings);
	}
}

void AutonomousDroid::setSensorsSubSystem(SensorsSubSystem *sensors)
{
	this->sensorsSubSystem = sensors;
	if (this->settings != nullptr) {
		sensorsSubSystem->setDroidSettings(this->settings);
	}
	this->sensorsSubSystem->setDebugMode(debugMode);
}

void AutonomousDroid::setEnginesSubSystem(EnginesSubSystem *engines)
{
	this->enginesSubSystem = engines;
	if (this->settings != nullptr) {
		enginesSubSystem->setSettings(this->settings);
	}
	this->enginesSubSystem->setDebugMode(debugMode);
	if (this->sensorsSubSystem != nullptr) {
		enginesSubSystem->setSensorsSubSystem(this->sensorsSubSystem);
	}
}

void AutonomousDroid::setNavigationSubSystem(NavigationSubSystem *navigation)
{
	this->navigationSubSystem = navigation;
	if (this->settings != nullptr) {
		this->navigationSubSystem->setSettings(this->settings);
	}
	this->navigationSubSystem->setDebugMode(debugMode);
	if (this->sensorsSubSystem != nullptr) {
		this->navigationSubSystem->setSensorsSubSystem(this->sensorsSubSystem);
	}
	if (i2cBus != nullptr) {
		this->navigationSubSystem->setI2CBus(i2cBus);
	}
}

void AutonomousDroid::setRechargeSubSystem(RechargeSubSystem *recharge)
{
	this->rechargeSubSystem = recharge;
	this->rechargeSubSystem->setDebugMode(debugMode);
	if (this->settings != nullptr) {
		this->rechargeSubSystem->setSettings(this->settings);
	}
	if (this->enginesSubSystem != nullptr) {
		this->rechargeSubSystem->setEnginesSubSystem(this->enginesSubSystem);
	}
	if (this->i2cBus != nullptr) {
		this->rechargeSubSystem->setI2CBus(i2cBus);
	}
	if (this->navigationSubSystem != nullptr) {
		this->rechargeSubSystem->setNavigationSubSystem(this->navigationSubSystem);
	}
}

void AutonomousDroid::setMazeMode(bool mode)
{
	isMazeMode = mode;
	if (this->navigationSubSystem != nullptr) {
		this->navigationSubSystem->setMazeMode(mode);
	}
	if (this->sensorsSubSystem != nullptr) {
		if (mode) {
			this->sensorsSubSystem->setServoPosition(180);
		}
		else {
			this->sensorsSubSystem->setServoPosition(90);
		}
	}
}

void AutonomousDroid::move()
{
	if (commandSubSystem != nullptr && commandSubSystem->isEnabledCommandISR()) {
		if (this->i2cBus != nullptr) {
			this->i2cBus->enableLCD();
		}
		if (commandSubSystem->isReady()) {
			commandSubSystem->reset(commandSubSystem->getCommand());
		}
		return;
	}
	if (this->i2cBus != nullptr) {
		this->i2cBus->disableLCD();
	}
	int position = navigationSubSystem->getDirection();
	if (position == -90) {
		if (debugMode) {
			Serial.println("left");
		}
	}
	else if (position == 180) {
		if (isMazeMode) {
			//we should go way back.
			for (int i = 0; i < 4; i++) {
				enginesSubSystem->goSlowBackward();
			}
		}
		enginesSubSystem->goSlowBackward();
		if (debugMode) {
			Serial.println("back");
		}
		return;
	}
	else if (position == 90) {
		if (debugMode) {
			Serial.println("right");
		}
	}
	else if (position == 0) {
		enginesSubSystem->goSlowForward();
		if (debugMode) {
			Serial.println("forward");
		}
		return;
	}
	enginesSubSystem->turnNrDegree(position);
}

void  AutonomousDroid::goToLight()
{
	if (commandSubSystem != nullptr && commandSubSystem->isEnabledCommandISR()) {
		if (commandSubSystem->isReady()) {
			commandSubSystem->reset(commandSubSystem->getCommand());
		}
		return;
	}
	if (rechargeSubSystem != nullptr) {
		rechargeSubSystem->goToLight();
	}
}