/*
 * CLogger.h
 *
 *  Created on: Oct 7, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef LOGGERS_CLOGGER_H_
#define LOGGERS_CLOGGER_H_
#include <string>

class CLogger {
public:
	CLogger();
	virtual ~CLogger();
	virtual void info(const char *data) = 0;
	virtual void debug(const char *data) = 0;
	virtual void error(const char *data) = 0;
	virtual void info(std::string data) = 0;
	virtual void debug(std::string data) = 0;
	virtual void error(std::string data) = 0;
	virtual int isDebug() = 0;
};

#endif /* LOGGERS_CLOGGER_H_ */
