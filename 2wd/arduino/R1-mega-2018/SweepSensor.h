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
#ifndef SweepSensor_h
#define SweepSensor_h
#include <Arduino.h>

class SweepSensor
{
public:
	SweepSensor();
	virtual unsigned long calculateDistance() = 0;
	void setMaxDistance(unsigned long maxDistance);
	void setDebugMode(bool debug);
protected:
	unsigned long MAX_DISTANCE = 0L;
	bool debugMode;
};
#endif