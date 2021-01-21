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

#include "ThreadExecutors.h"
#include<utility>
#include <vector>
#include <stdio.h>

ThreadExecutors::ThreadExecutors()
{
	m_threads = new std::map<std::string, pthread_t>();
	m_executors = new std::map<std::string, ThreadExecutor*>();
}

ThreadExecutors::~ThreadExecutors()
{
	for(std::map<std::string, pthread_t>::iterator it=m_threads->begin(); it != m_threads->end(); ++it)
	{
		pthread_t thread = it->second;
		if (thread != NULL)
		{
			pthread_cancel(thread);
			ThreadExecutor *executor = m_executors->at(it->first);
			executor->stop();
			pthread_join(thread, NULL);
		}
	}
	m_threads->clear();
	delete m_threads;
	//clear the executors
	std::vector<std::string> ids;
	for(std::map<std::string, ThreadExecutor*>::iterator it = m_executors->begin(); it != m_executors->end(); ++it)
	{
		ids.push_back(it->first);
	}
	for (std::vector<std::string>::iterator it = ids.begin(); it != ids.end(); ++it)
	{
		delete m_executors->at(*it);
	}
	m_executors->clear();
	delete m_executors;
}

void ThreadExecutors::addExecutor(std::string t_id, ThreadExecutor *t_executor)
{
	m_executors->insert(std::make_pair(t_id, t_executor));
}

void ThreadExecutors::startExecutor(std::string t_id)
{
	ThreadExecutor* executor = m_executors->at(t_id);
	if (executor != nullptr)
	{
		pthread_t thread;
		if (pthread_create(&thread, NULL, ThreadExecutors::execute, executor) == 0)
		{
			m_threads->insert(std::make_pair(t_id, thread));
		} else {
			printf("Could not start thread with id=%s\n", t_id.c_str());
			fflush(stdout);
		}
	} else {
		printf("Executor with id=%s is not registered\n", t_id.c_str());
		fflush(stdout);
	}
}

void ThreadExecutors::stopExecutor(std::string t_id)
{
	if (m_threads->empty()) {
		return;
	}
	pthread_t thread = m_threads->at(t_id);
	if (thread != NULL)
	{
		ThreadExecutor *executor = m_executors->at(t_id);
		executor->stop();
		pthread_join(thread, NULL);
		m_threads->erase(t_id);
	}
}

void *ThreadExecutors::execute(void *t_arg)
{
	if(t_arg != nullptr)
	{
		((ThreadExecutor *)t_arg)->start();
    } else {
    	printf("Argument for thread executors is null\n");
    	fflush(stdout);
    }
	return nullptr;
}

void ThreadExecutors::deleteExecutor(std::string t_id)
{
	ThreadExecutor* executor = m_executors->at(t_id);
	if (executor != nullptr)
	{
		m_executors->erase(t_id);
		delete executor;
	} else {
		printf("Executor with t_id %s doesn't exist\n", t_id.c_str());
		fflush(stdout);
	}
}
