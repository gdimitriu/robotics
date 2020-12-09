/*
 * detecti2c.c
 *
 *  Created on: Sep 22, 2020
 *      Author: Gabriel Dimitriu
 * 
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
		int address, error;
		for(address = 1; address < 127; address++) {
			int handle = i2cOpen(atoi(argv[1]), address, 0);
			printf("Device found at adress 0x");
			if( handle > 0 && i2cClose(handle)) {
				if (address < 16) {
					printf("0");
				}
				printf("%x %d\n", address, address);
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
