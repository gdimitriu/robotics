/*
 * led_gpio.c
 *
 *  Created on: Sep 22, 2020
 *      Author: Gabriel Dimitriu
 * 
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
