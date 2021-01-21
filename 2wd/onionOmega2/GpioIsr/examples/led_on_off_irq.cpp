/*
Led on specific GPIO is on then off after soft irq.
This use the fast-gpio library from: https://github.com/OnionIoT/fast-gpio
This is used for testing GPIO of omega, is full verbose:
- it get the direction of the pin
- set the output direction of the pin
- wait for irq on pin
- led in on
- delay 2 seconds
- led if off
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

#include <fastgpioomega2.h>
#include <fastgpio.h>
#include <ExGpioIsrObserver.h>
#include <GpioIsr.h>

int main(int argc, char *argv[]) {

	FastGpio	* gpioObj = new FastGpioOmega2();
	if (argc != 4) {
		printf("Usage:\n");
		printf("%s outputPinNr inputPinNr edgeInputPin[0,1]\n", argv[0]);
		printf("edge is 0 for falling and 1 for rising\n\n");
			exit(-1);
	}
	ExGpioIsrObserver *observer = new ExGpioIsrObserver(gpioObj, atoi(argv[1]));
	gpioObj->SetVerbosity(1);	
	GpioIsr *isr = new GpioIsr(atoi(argv[2]), atoi(argv[3]), observer, 3 * 1000);
	isr->start();
}