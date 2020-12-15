/*
 * CResetableBarrier.h
 *
 *  Created on: Nov 20, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
 *
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

#ifndef CONTROL_ENGINES_CRESETABLEBARRIER_H_
#define CONTROL_ENGINES_CRESETABLEBARRIER_H_
#include <pthread.h>

class CResetableBarrier {
public:
	CResetableBarrier();
	void init(unsigned int threshold);
	void sync();
	void reset();
	void setThreshold(unsigned int threshold);
	virtual ~CResetableBarrier();
private:
	pthread_mutex_t m_barrier_lock;
	pthread_cond_t m_barrier_condvar;
	unsigned int m_barrier_count;
	unsigned int m_barrier_threshold;
};

#endif /* CONTROL_ENGINES_CRESETABLEBARRIER_H_ */
