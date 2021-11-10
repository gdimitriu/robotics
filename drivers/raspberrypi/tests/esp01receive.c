/*
 * esp01receive.c
 *
 *  Created on: Nov 2, 2021
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2021 Gabriel Dimitriu
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
#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <string.h>

void printData(int serialHandler) {
	char buffer[255];
	char sendBuffer[255];
	if (serDataAvailable(serialHandler) > 0) {
		memset(buffer, 0, sizeof(buffer));
		int readnr = serRead(serialHandler, buffer, 255);
		buffer[readnr + 1] = '\0';
		printf("%s\n", buffer);
		fflush(stdout);
	}
}

void recvData(int serialHandler) {
	char buffer[255];
	char sendBuffer[255];
	int i,j;
	if (serDataAvailable(serialHandler) > 0) {
		memset(buffer, 0, sizeof(buffer));
		int readnr = serRead(serialHandler, buffer, 255);
		buffer[readnr + 1] = '\0';
		if (strcmp(buffer, "exit") == 0) {
			serClose(serialHandler);
			gpioTerminate();
			exit(0);
		}
		printf("received<<%s<<endReceive\n", buffer);
		fflush(stdout);
		int commandPos = 0;
		for (i = 0 ;i < readnr; i++) {
			if (buffer[i] == ':') {
				commandPos = i + 1;
				break;
			}
		}
		for (i = commandPos,j=0; i < readnr; i++, j++) {
			if (buffer[i] == '#') {
				buffer[j] = buffer[i];
				buffer[++j] = '\0';
				break;
			}
			buffer[j] = buffer[i];
		}
		memset(&buffer[j],'\0',sizeof(buffer)-sizeof(char)*j);
		printf("%s\n", buffer);
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer,"AT+CIPCLOSE=0");
		serWrite(serialHandler,sendBuffer,strlen(sendBuffer));
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s ttynr\n", argv[0]);
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	}
	int serialHandler = serOpen(argv[1], 115200, 0);
	if (serialHandler < 0) {
		perror("gpio serial open failed");
		gpioTerminate();
		exit(1);
	}
	printf("serial is succesfully open\n");
	fflush(stdout);
	char message[256];
	sprintf(message,"%s\r\n","ATE0");
	serWrite(serialHandler, message, strlen(message));
	printf("Sucessfully send %s",message);
	fflush(stdout);
	sleep(1);
	printData(serialHandler);
	memset(message, 0, sizeof(message));
	sprintf(message,"%s\r\n","AT+CWMODE=3");
	serWrite(serialHandler, message, strlen(message));
	printf("Sucessfully send %s",message);
	fflush(stdout);
	sleep(1);
	printData(serialHandler);
	memset(message, 0, sizeof(message));
	sprintf(message,"%s\r\n","AT+CIPMUX=1");
	serWrite(serialHandler, message, strlen(message));
	printf("Sucessfully send %s",message);
	fflush(stdout);
	sleep(1);
	printData(serialHandler);
	memset(message, 0, sizeof(message));
	sprintf(message,"%s\r\n","AT+CIPSERVER=1,8080");
	serWrite(serialHandler, message, strlen(message));
	printf("Sucessfully send %s",message);
	fflush(stdout);
	sleep(1);
	printData(serialHandler);
	memset(message, 0, sizeof(message));
	sprintf(message,"%s\r\n","AT+CIFSR");
	serWrite(serialHandler, message, strlen(message));
	printf("Sucessfully send %s",message);
	fflush(stdout);
	sleep(1);
	printData(serialHandler);
//	sprintf(message,"%s\r\n","AT+CIPRECVMODE=0");
//	serWrite(serialHandler, message, strlen(message));
//	printf("Sucessfully send %s",message);
//	fflush(stdout);
//	sleep(1);
//	printData(serialHandler);
//
	while (1) {
		recvData(serialHandler);
		sleep(1);
	}
}

