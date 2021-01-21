/*
Thread executors for onion omega ISR and other concurent operations.
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

#ifndef ThreadExecutors_h
#define ThreadExecutors_h

#include "ThreadExecutor.h"
#include <string>
#include <map>
#include <pthread.h>

class ThreadExecutors
{
public:
	ThreadExecutors();
	virtual ~ThreadExecutors();
	void addExecutor(std::string t_id, ThreadExecutor *t_executor);
	void startExecutor(std::string t_id);
	static void *execute(void *t_arg);
	void stopExecutor(std::string t_id);
	void deleteExecutor(std::string t_id);
private:
	std::map<std::string, ThreadExecutor *> *m_executors;
	std::map<std::string, pthread_t> *m_threads;
};

#endif
