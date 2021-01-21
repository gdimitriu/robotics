/*
 * loadTest.cpp
 *
 *  Created on: Oct 5, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
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
/*
 * in qemu run like:
 * qemu-arm -E LD_LIBRARY_PATH=${PREFIX_GPIO}:${PREFIX_DRIVERS}/lib:../lib:/home/onion/raspberrypi/tools/arm-bcm2708/arm-rpi-4.9.3-linux-gnueabihf/arm-linux-gnueabihf/sysroot/
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <CEngineWithEncoder.h>
#include <CFactoryEngine.h>
#include <CFactoryEnginePCA9685EncMX1508.h>
#include <CLoggerStdout.h>
#include <CSettingLoading.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " configuration_file\n";
		exit(1);
	}
	CSettingLoading *settings = new CSettingLoading(argv[1],new CLoggerStdout());
	char *type = settings->getLine();
	CLogger *logger;
	if (strcmp("CLoggerStdout", type) == 0) {
		logger = new CLoggerStdout();
	}
	type = settings->getLine();
	CFactoryEngine *factory;
	if (strcmp("CFactoryEnginePCA9685EncMX1509", type) == 0) {
		char *type = settings->getLine(); //ignore the pwmDriver
		factory = new CFactoryEnginePCA9685EncMX1508(settings, NULL, logger);
	}
	else
		exit(1);
	unsigned int nrEngines = 0;
	CEngineWithEncoder **engines = factory->createEngines(nrEngines);
	std::cout<<"Number of engines="<<nrEngines<<std::endl;
	exit(0);
}

