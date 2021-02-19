/*
 * detecti2c.c
 *
 *  Created on: Sep 22, 2020
 *      Author: Gabriel Dimitriu
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


#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr,"Usage: %s i2cbus\n", argv[0]);
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		printf("I2C Scanner\n");
		int nDevices  = 0;
		int address;
		for(address = 1; address < 127; address++) {
			int handle = i2cOpen(atoi(argv[1]), address, 0);
			if( (handle > 0) && (i2cClose(handle) == 0)) {
				printf("Device found at address 0x%x %d\n", address, address);
				nDevices++;
			}
		}
		if (nDevices == 0) {
			printf("No i2c devices found\n");
		}
		gpioTerminate();
		exit(0);
	}
}
