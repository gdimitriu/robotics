/*
 * trigger_gpio.c
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
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <chrono>

int exitCond = 0;

int main(int argc, char **argv) {

	if (argc != 3) {
		fprintf(stderr,"Usage: %s port_echo port_trig)\n", argv[0]);
		exit(1);
	}
	unsigned port2 = atoi(argv[2]);
	unsigned port1 = atoi(argv[1]);
	if ( port1 < 0 || port1 > 32) {
		fprintf(stderr,"Port_echo should be between 0 and 31\n");
		exit(1);
	}
	if ( port2 < 0 || port2 > 32) {
		fprintf(stderr,"Port_trig should be between 0 and 31\n");
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		gpioSetMode(port1, PI_INPUT);
		gpioSetMode(port2, PI_OUTPUT);
		gpioSetPullUpDown(port1, PI_PUD_UP);
		int d = 1;
		std::chrono::microseconds timeout(26190); //450 cm ...
		while(d == 1) {
			gpioWrite(port2,0);
			usleep(2);
			gpioWrite(port2,1);
			usleep(10);
			gpioWrite(port2,0);
			int timeoutOccured = 0;
			while(gpioRead(port1) == 0);
			auto start = std::chrono::high_resolution_clock::now();
			while(gpioRead(port1) == 1) {
				if ((std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()-start)) > timeout) {
					timeoutOccured = 1;
					printf("Timeout here\n");
					fflush(stdout);
					break;
				}
			}
			auto finish = std::chrono::high_resolution_clock::now();
			if (timeoutOccured == 0) {
				float microseconds = std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count();
				std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count() << "us\n";
				printf("Distance = %f\n", (microseconds/2)/29.1);
			} else {
				printf("Timeout\n");
			}
			printf("next ?0/1 = ");
			scanf("%d",&d);
			fflush(stdout);
		}
		gpioTerminate();
		exit(0);
	}
}
