/*
 * CLoggerBle.h
 *
 *  Created on: Jan 4, 2021
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2021 Gabriel Dimitriu
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

#ifndef LOGGERS_CLOGGERBLE_H_
#define LOGGERS_CLOGGERBLE_H_

#include "CLogger.h"

class CLoggerBle: public CLogger {
public:
	CLoggerBle();
	CLoggerBle(char *tty,int boudRate);
	virtual ~CLoggerBle();
	virtual void info(const char *data);
	virtual void debug(const char *data);
	virtual void error(const char *data);
	virtual void info(std::string data);
	virtual void debug(std::string data);
	virtual void error(std::string data);
protected:
	int m_serialHandler;
};

#endif /* LOGGERS_CLOGGERBLE_H_ */
