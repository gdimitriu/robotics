/*
 * ble_serial_receive.c
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
	char buffer[255];
	char sendBuffer[255];
	while (1) {
		memset(sendBuffer, '\0', sizeof(sendBuffer));
		memset(buffer, '\0', sizeof(buffer));
		scanf("%s", buffer);
		printf("%s end\n", buffer);
		fflush(stdin);
		sprintf(sendBuffer,"%s\n", buffer);
		serWrite(serialHandler, sendBuffer, strlen(sendBuffer) + 1);
		printf("%s end send\n", buffer);
		fflush(stdout);
		if (strcmp(buffer, "exit") == 0) {
			serClose(serialHandler);
			gpioTerminate();
			exit(0);
		}
	}
}
