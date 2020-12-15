/*
 * ble_serial_receive.c
 *
 *  Created on: Sep 26, 2020
 *      Author: Gabriel Dimitriu
 * you have to add into /boot/config.txt at tag all
 * dtoverlay=disable-bt
 * enable_uart=1
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


#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <string.h>

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr,"Usage: %s ttynr\n", argv[0]);
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	}
	int serialHandler = serOpen(argv[1], 38400, 0);
	if (serialHandler < 0) {
		perror("gpio serial open failed");
		gpioTerminate();
		exit(1);
	}
	char *message = "started\n";
	serWrite(serialHandler,message, strlen(message));
	char buffer[255];
	char sendBuffer[255];
	while(1) {
		if (serDataAvailable(serialHandler) > 0) {
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(serialHandler, buffer, 255);
			buffer[readnr + 1] = '\0';
			if (strcmp(buffer,"exit") == 0 ) {
				serClose(serialHandler);
				gpioTerminate();
				exit(0);
			}
			printf("%s\n", buffer);
			fflush(stdout);
			memset(sendBuffer, 0, sizeof(sendBuffer));
			sprintf(sendBuffer, "%s received\n", buffer);
			serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
		}
		sleep(1);
	}
}
