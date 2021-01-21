/*
Observer for interrupt service routine gpio on omega2.
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
#ifndef ExGpioIsrObserverPrint_h
#define ExGpioIsrObserverPrint_h

#include <string.h>
#include <map>
#include "GpioIsrObserver.h"

class ExGpioIsrObserverPrint : public GpioIsrObserver
{
public:
	ExGpioIsrObserverPrint(std::string *t_name);
	virtual ~ExGpioIsrObserverPrint();
	virtual void observe();
private:
	int m_countValue;
	std::string *m_name;
};
#endif
