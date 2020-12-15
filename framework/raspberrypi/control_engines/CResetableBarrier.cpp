/*
 * CResetableBarrier.cpp
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

#include "CResetableBarrier.h"

CResetableBarrier::CResetableBarrier() {
	m_barrier_count = 0;
	m_barrier_threshold = 0;
}

CResetableBarrier::~CResetableBarrier() {
	pthread_cond_broadcast(&m_barrier_condvar);
	pthread_mutex_destroy(&m_barrier_lock);
	pthread_cond_destroy(&m_barrier_condvar);
}

void CResetableBarrier::init(unsigned int threshold) {
	pthread_mutex_init(&m_barrier_lock, NULL);
	pthread_cond_init(&m_barrier_condvar, NULL);
	m_barrier_count = 0;
	m_barrier_threshold = threshold;
}

void CResetableBarrier::sync() {
	pthread_mutex_lock(&m_barrier_lock);
	m_barrier_count++;
	if (m_barrier_count < m_barrier_threshold) {
			pthread_cond_wait(&m_barrier_condvar, &m_barrier_lock);
	} else {
		m_barrier_count = 0;
		for (int i = m_barrier_threshold; i > 0; i--)
			pthread_cond_signal(&m_barrier_condvar);
	}
	pthread_mutex_unlock(&m_barrier_lock);
}

void CResetableBarrier::reset() {
	m_barrier_count = 0;
	pthread_cond_broadcast(&m_barrier_condvar);
}

void CResetableBarrier::setThreshold(unsigned int threshold) {
	reset();
	m_barrier_threshold = threshold;
}
