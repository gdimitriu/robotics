/*
 * trigger_gpio.cpp
 *
 *  Created on: Oct 2, 2020
 *      Author: Gabriel Dimitriu
 * 
 */

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pigpio.h>
#include "CIsr.h"
using namespace std;

int CIsr::count = 0;

int main(int argc, char **argv) {

	if (argc != 4) {
		cerr<<"Usage: "<<argv[0]<<"port1 port 2 type (0 rising, 1, falling, 2 both)\n";
		exit(1);
	}
	unsigned type = atoi(argv[3]);
	if ( atoi(argv[1]) < 0 || atoi(argv[1]) > 54) {
			cerr<<"Port should be between 0 and 53 but is "<<argv[1]<<endl;
			exit(1);
	}
	if ( atoi(argv[2]) < 0 || atoi(argv[2]) > 54) {
				cerr<<"Port should be between 0 and 53 but is "<<argv[2]<<endl;
				exit(1);
		}
	if (type < 0 || type > 2) {
		cerr<<"Type should be 0 rising, 1, falling, 2 both\n";
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		CIsr *firstIsr = new CIsr(atoi(argv[1]), atoi(argv[3]));
		CIsr *secondIsr = new CIsr(atoi(argv[2]), atoi(argv[3]));
		firstIsr->startIsr();
		secondIsr->startIsr();
		while(CIsr::count < 100) sleep(5);
		gpioTerminate();
		exit(0);
	}
}
