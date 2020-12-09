/*
 * trigger_gpio.c
 *
 *  Created on: Sep 22, 2020
 *      Author: Gabriel Dimitriu
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

int exitCond = 0;

void isrCallback (int gpio, int level, uint32_t tick) {

	fprintf(stdout,"ISR on port %d with level %d\n", gpio, level);
	exitCond = 1;
}

int main(int argc, char **argv) {

	if (argc != 3) {
		fprintf(stderr,"Usage: %s port type (0 rising, 1, falling, 2 both)\n", argv[0]);
		exit(1);
	}
	unsigned type = atoi(argv[2]);
	unsigned port = atoi(argv[1]);
	if ( port < 0 || port > 54) {
			fprintf(stderr,"Port should be between 0 and 53\n");
			exit(1);
	}
	if (type < 0 || type > 2) {
		fprintf(stderr, "Type should be 0 rising, 1, falling, 2 both\n");
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		unsigned int realType;
		switch(type){
		case 0:
			realType = RISING_EDGE;
			break;
		case 1:
			realType = FALLING_EDGE;
			break;
		case 2:
			realType = EITHER_EDGE;
			break;
		default:
			realType = RISING_EDGE;
		}
		gpioSetMode(port, PI_INPUT);
		gpioSetPullUpDown(port, PI_PUD_UP);
		gpioSetISRFunc(port, realType, -1, isrCallback);
		while(!exitCond) sleep(5);
		gpioTerminate();
		exit(0);
	}
}
