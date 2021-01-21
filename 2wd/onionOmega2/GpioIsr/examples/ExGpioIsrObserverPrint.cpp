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
#include "ExGpioIsrObserverPrint.h"

void ExGpioIsrObserverPrint::observe() 
{
	m_countValue++;
	printf("%s : %d\n",m_name->c_str(), m_countValue); fflush(stdout);
}

ExGpioIsrObserverPrint::ExGpioIsrObserverPrint(std::string *t_name)
{
	this->m_name = t_name;
	m_countValue = 0;
}

ExGpioIsrObserverPrint::~ExGpioIsrObserverPrint()
{
	delete m_name;
}
