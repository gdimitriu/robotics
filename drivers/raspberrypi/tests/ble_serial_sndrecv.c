/*
 * ble_serial_sndrecv.c
 *
 *  Created on: Sep 26, 2020
 *      Author: Gabriel Dimitriu
 * you have to add into /boot/config.txt at tag all
 * dtoverlay=disable-bt
 * enable_uart=1
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>
#include <string.h>
#include <sys/types.h>

int exitMode = 0;

void* receiveMode(void *data) {
	int serialHandler = *(int *)data;
	char buffer[255];
	char sendBuffer[255];
	while (exitMode == 0) {
		if (serDataAvailable(serialHandler) > 0) {
			memset(sendBuffer, 0, sizeof(sendBuffer));
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(serialHandler, buffer, 255);
			buffer[readnr + 1] = '\0';
			if (strcmp(buffer, "exit") == 0) {
				exitMode = 1;
				serClose(serialHandler);
				gpioTerminate();
				exit(0);
			}
			printf("%s\n", buffer);
			fflush(stdout);
			memset(sendBuffer, 0, sizeof(sendBuffer));
			sprintf(sendBuffer, "%s received\n", buffer);
			serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
		} else {
			sleep(1);
		}
	}
	serClose(serialHandler);
}

void* sendMode(void *data) {
	int serialHandler = *(int *)data;
	char sendBuffer[255];
	char buffer[255];
	while (exitMode == 0) {
		memset(sendBuffer, 0, sizeof(sendBuffer));
		memset(buffer, 0, sizeof(buffer));
		scanf("%s", buffer);
		fflush(stdin);
		sprintf(sendBuffer, "%s\n", buffer);
		serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
		if (strcmp(buffer, "exit") == 0) {
			exitMode = 1;
			serClose(serialHandler);
			gpioTerminate();
			exit(0);
		}
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
	int serialHandler = serOpen(argv[1], 38400, 0);
	if (serialHandler < 0) {
		perror("gpio serial open failed");
		gpioTerminate();
		exit(1);
	}

	pthread_t threads[2];
	int i;

	pthread_create(&threads[0], NULL, receiveMode, &serialHandler);
	pthread_create(&threads[1], NULL, sendMode, &serialHandler);

	for (i = 0; i < 2; i++) {
		int s = pthread_join(threads[i], NULL);
	}
}
