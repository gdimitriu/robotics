/*
 * CIsr.h
 *
 *  Created on: Oct 1, 2020
 *      Author: Gabriel Dimitriu
 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
