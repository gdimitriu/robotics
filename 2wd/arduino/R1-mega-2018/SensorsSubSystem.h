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
#ifndef SensorsSubSystem_h
#define SensorsSubSystem_h
#include <Arduino.h>
#include <Servo.h>
#include "AutonomousDroidSettings.h"
#include "Observer.h"
#include "SweepSensor.h"

class SensorsSubSystem : public Observer
{
public:
	SensorsSubSystem();
	void setDroidSettings(AutonomousDroidSettings *settings);
	void setDebugMode(bool debug);	
	void attachFrontLeftSensor(int sensorPin, int detectState);
	void attachFrontCentralSensor(int sensorPin, int detectState);
	void attachFrontRightSensor(int sensorPin, int detectState);
	void attachFrontSweepSensor(int servoPin, SweepSensor *frontSensor);
	void attachReverseSensor(int sensorPin, int detectState);
	void setServoPosition(int position);
	unsigned long calculateDistanceSweepSensor();
	bool hasFrontSensors();
	void initialize();
	virtual void update();
	//readers
	/*
	* Get the readings from front sensors (left-center-right sensors)
	*/
	void readFrontSensors(bool &lefFrontSensor, bool &centralFrontSensor, bool &rightFrontSensor);
	/*
	* Pan the senzor in the position and get the distance reading.
	*/
	unsigned long look(int position);
	/*
	* Get the left and right distances from sweep sensor.
	* input lef and right option from fixed sensors
	* return in second and third argument the distances
	*/
	void getLeftRightDistances(unsigned long &leftDistance, unsigned long &rightDistance);
	bool isFrontLeftObstructed();
	bool isFrontCentralObstructed();
	bool isFrontRightObstructed();
	bool isReverseObstructed();
protected:
	bool debugMode = false;
	//statical sensors
	int pinFrontLeftSensor = -1;
	int detectFrontLeftSensor = 0;
	int pinFrontCentralSensor = -1;
	int detectFrontCentralSensor = 0;
	int pinFrontRightSensor = -1;
	int detectFrontRightSensor = 0;
	int pinReverseSensor = -1;
	int detectReverseSensor = 0;
	
	/*
	* Moving sensors
	*/
	SweepSensor *frontSweepSensor;
	//pin for the pan senzor
	int LOOK_SERVO_PIN = 0;
	//servo instance
	Servo sensorServo;
	int oldServoPosition = 90; //center position
	AutonomousDroidSettings *droidSettings = nullptr;
};

#endif
