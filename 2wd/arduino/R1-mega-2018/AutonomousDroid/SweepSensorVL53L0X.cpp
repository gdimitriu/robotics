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
#include "SweepSensorVL53L0X.h"

SweepSensorVL53L0X::SweepSensorVL53L0X(char address)
{
	Wire.begin();
	sensor.setAddress(address);
	sensorAddress = address;
	sensor.init();
	sensor.setTimeout(500);
	// lower the return signal rate limit (default is 0.25 MCPS)
	sensor.setSignalRateLimit(0.1);
	// increase laser pulse periods (defaults are 14 and 10 PCLKs)
	sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
	sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
}

unsigned long SweepSensorVL53L0X::calculateDistance()
{
	unsigned long value = sensor.readRangeSingleMillimeters();
	if (sensor.timeoutOccurred()) {
		return MAX_DISTANCE;
	}
	return value;
}

char SweepSensorVL53L0X::getAddress()
{
	return sensorAddress;
}