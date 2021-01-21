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
#ifndef ExGpioIsrObserver_h
#define ExGpioIsrObserver_h

#include <GpioIsrObserver.h>
#include <fastgpio.h>

class ExGpioIsrObserver : public GpioIsrObserver
{
public:
	ExGpioIsrObserver(FastGpio *t_fastGpio, unsigned int t_pinNr);
	virtual ~ExGpioIsrObserver();
	virtual void observe();
private:
	FastGpio * m_fastGpio;
	unsigned int m_pinNr;
	int m_originalDirection;
	int m_direction;
};
#endif
