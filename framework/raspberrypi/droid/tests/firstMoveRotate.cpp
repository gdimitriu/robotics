/*
 * firstRotate.cpp
 *
 *  Created on: Oct 12, 2020
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

#include <iostream>
#include <CDroid.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr<<"Usage="<<argv[0]<<" configFile\n";
		exit(1);
	}
	char buff[255];
	CDroid *droid = new CDroid(argv[1], 0);
	droid->dumpInfo();
	while(1) {
		printf("Enter the distance and rotation!\n");
		fflush(stdout);
		int degree;
		float dinstance;
		scanf("%f %d",&dinstance, &degree);
		droid->move(dinstance, degree, 0);
		printf("enter a string to stop, exit to exit program\n");
		fflush(stdout);
		scanf("%s",&buff);
		fflush(stdin);
		droid->fullStop();
		if (strcmp("exit", buff) == 0) {
			droid->fullStop();
			delete droid;
			exit(0);
		}
	}
}


