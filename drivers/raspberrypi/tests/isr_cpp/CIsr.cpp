/*
 * CIsr.cpp
 *
 *  Created on: Oct 1, 2020
 *      Author: Gabriel Dimitriu
 * 
 */

#include "CIsr.h"
#include <iostream>
#include <stdlib.h>
using namespace std;

CIsr::CIsr() {
	this->gpio = 0;
	this->level = RISING_EDGE;
}

CIsr::CIsr(int gpio, int level) {
	this->gpio = gpio;
	switch (level) {
	case 0:
		this->level = RISING_EDGE;
		break;
	case 1:
		this->level = FALLING_EDGE;
		break;
	case 2:
		this->level = EITHER_EDGE;
		break;
	default:
		this->level = RISING_EDGE;
	}
}

CIsr::~CIsr() {
	gpioSetISRFuncEx(gpio, level, -1, NULL, this);
}

void CIsr::startIsr() {
	gpioSetMode(gpio, PI_INPUT);
	gpioSetPullUpDown(gpio, PI_PUD_UP);
	gpioSetISRFuncEx(gpio, level, -1, CIsr::ISRMain, this);
}

void CIsr::ISRMain(int gpio, int level, uint32_t tick, void *holder) {
	cout << "Received isr on " << gpio << endl;
	((CIsr*) holder)->ISR(gpio, level, tick);
}

void CIsr::ISR(int gpio, int level, uint32_t tick) {
	cout << "Received isr in holder on " << gpio << endl;
	count++;
}
