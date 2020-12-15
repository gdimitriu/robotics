/*
 * led_gpio.c
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


#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	if (argc != 3) {
		fprintf(stderr,"Usage: %s port 0/1\n", argv[0]);
		exit(1);
	}
	unsigned port =atoi(argv[1]);
	unsigned value = atoi(argv[2]);
	if ( port < 0 || port > 54) {
		fprintf(stderr,"Port should be between 0 and 53\n");
		exit(1);
	}
	if (value < 0 || value > 1) {
		fprintf(stderr, "Value should be 0 or 1\n");
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		gpioSetMode(port, PI_OUTPUT);
		gpioWrite(port, value);
		gpioTerminate();
		exit(0);
	}
}
