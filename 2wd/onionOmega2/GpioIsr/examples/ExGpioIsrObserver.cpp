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
#include <stdio.h>
#include <unistd.h>
#include <ExGpioIsrObserver.h>

void ExGpioIsrObserver::observe() 
{
	printf("ISR GOT\n"); fflush(stdout);
	m_fastGpio->Set(m_pinNr, 1);
	sleep(4);
	m_fastGpio->Set(m_pinNr, 0);
}

ExGpioIsrObserver::ExGpioIsrObserver(FastGpio *t_fastGpio, unsigned int t_pinNr)
{
	this->m_fastGpio = t_fastGpio;
	this->m_pinNr = t_pinNr;
	m_originalDirection = 0;
	m_direction = 0;
	m_fastGpio->GetDirection(t_pinNr, m_originalDirection);
	m_fastGpio->SetDirection(t_pinNr, 1);
}

ExGpioIsrObserver::~ExGpioIsrObserver()
{
	m_fastGpio->Set(m_pinNr, 0);
	m_fastGpio->SetDirection(m_pinNr, m_originalDirection);
}
