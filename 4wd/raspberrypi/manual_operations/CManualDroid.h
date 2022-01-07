/*
 * CManualDroid.h
 *
 *  Created on: Nov 29, 2020
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

#ifndef MANUAL_OPERATIONS_CMANUALDROID_H_
#define MANUAL_OPERATIONS_CMANUALDROID_H_
#include <CDroid.h>
#include <CCommCommands.h>
#include <vector>
#include <pthread.h>

class CManualDroid : public CDroid {
public:
	CManualDroid(char *droidCfgFile, int isOnHost,vector<CCommCommands *> *commands);
	virtual ~CManualDroid();
	virtual void dumpInfo();
	virtual void startReceiving();
	virtual void stopReceiving();
protected:
	friend void *manualDroidCommandExecute(void *command);
	vector<CCommCommands *> *m_commands;
	pthread_attr_t m_attr;
	pthread_t *m_threads;
};

#endif /* MANUAL_OPERATIONS_CMANUALDROID_H_ */
