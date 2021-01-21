/*
Two IR interrupt simulation for encoders.
This use the fast-gpio library from: https://github.com/OnionIoT/fast-gpio
This is used for testing GPIO of omega, is full verbose:
- it get the direction of the pin
- set the output direction of the pin
- wait on thread for irq on pin 1 print it
- wait on thread for irq on pin 2 print it
- set the original direction of the pin

Copyright (C) 2019 Gabriel Dimitriu
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include <ExGpioIsrObserverPrint.h>
#include <GpioIsr.h>
#include <ThreadExecutors.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

	ThreadExecutors executors;
	if (argc != 5) {
		printf("Usage:\n");
		printf("%s inputPinNr1 inputPinNr2 edgeInputPin[0,1] timeout\n", argv[0]);
		printf("edge is 0 for falling and 1 for rising\n\n");
		exit(-1);
	}
	ExGpioIsrObserverPrint *observer1 = new ExGpioIsrObserverPrint(new std::string("first"));
	ExGpioIsrObserverPrint *observer2 = new ExGpioIsrObserverPrint(new std::string("second"));
	GpioIsr *isr1 = new GpioIsr(atoi(argv[1]), atoi(argv[3]), observer1, atoi(argv[4]));
	GpioIsr *isr2 = new GpioIsr(atoi(argv[2]), atoi(argv[3]), observer2, atoi(argv[4]));
	executors.addExecutor("first", isr1);
	executors.addExecutor("second", isr2);
	executors.startExecutor("first");
	executors.startExecutor("second");
	int ch = getchar();
	executors.stopExecutor("first");
	executors.stopExecutor("second");
	executors.deleteExecutor("first");
	executors.deleteExecutor("second");
}
