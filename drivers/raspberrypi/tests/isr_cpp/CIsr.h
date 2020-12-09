/*
 * CIsr.h
 *
 *  Created on: Oct 1, 2020
 *      Author: Gabriel Dimitriu
 * 
 */
#include <pigpio.h>

#ifndef TESTS_ISR_CPP_CISR_H_
#define TESTS_ISR_CPP_CISR_H_

class CIsr {
public:
	CIsr();
	CIsr(int gpio, int level);
	virtual ~CIsr();
	void startIsr();
	static void ISRMain(int gpio, int level, uint32_t tick, void *holder);
	void ISR(int gpio, int level, uint32_t tick);
	static int count;
private:
	int gpio;
	int level;
};

#endif /* TESTS_ISR_CPP_CISR_H_ */
