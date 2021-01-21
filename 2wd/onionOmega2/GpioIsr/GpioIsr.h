/*
Interrupt service routine gpio on omega2 based on https://github.com/OnionIoT/GpioIsr
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

#ifndef  GpioIsr_h
#define GpioIsr_h

#include <poll.h>
#include <ThreadExecutor.h>
#include <GpioIsrObserver.h>
#include <iostream>

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define MAX_BUF 64
class GpioIsr : public ThreadExecutor {
public:
	/****************************************************************
	* edge is 0 for falling 1 for rising and 2 for both.
	****************************************************************/
	GpioIsr(unsigned int t_pin, unsigned int t_edge, GpioIsrObserver *t_observer,int t_poolTimeout);

	virtual ~GpioIsr();

	int start();
	int stop();
protected:
	/****************************************************************
	* gpioExport
	****************************************************************/
	int gpioExport(unsigned int t_gpio);
	/****************************************************************
	* unexport
	****************************************************************/
	int gpioUnexport(unsigned int t_gpio);
	/****************************************************************
	* setDir
	****************************************************************/
	int setDir(unsigned int t_gpio, unsigned int t_out_flag);
	/****************************************************************
	* setValue
	****************************************************************/
	int setValue(unsigned int t_gpio, unsigned int t_value);
	/****************************************************************
	* getValue
	****************************************************************/
	int getValue(unsigned int t_gpio, unsigned int *t_value);
	/****************************************************************
	* setEdge
	****************************************************************/
	int setEdge(unsigned int t_gpio, char *t_edge);
	/****************************************************************
	* fdOpen
	****************************************************************/
	int fdOpen(unsigned int t_gpio);
	/****************************************************************
	* fdClose
	****************************************************************/
	int fdClose(int t_fd);
private:
	struct pollfd m_fdset[1];
	int m_nfds = 1;
	int m_gpio_fd;
	int m_timeout;
	char *m_buf[MAX_BUF];
	int m_gpio;
	GpioIsrObserver *m_observer;
	bool m_stopRequest;
	unsigned int m_edge;
};
#endif // ! GpioIsr.h

