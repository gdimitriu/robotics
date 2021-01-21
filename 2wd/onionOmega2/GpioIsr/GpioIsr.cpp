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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "GpioIsr.h"

GpioIsr::GpioIsr(unsigned int t_pin, unsigned int t_edge, GpioIsrObserver *t_observer,
		int t_poolTimeout) {
	m_gpio = t_pin;
	this->m_observer = t_observer;
	this->m_timeout = t_poolTimeout;
	this->m_edge = t_edge;
	this->m_gpio_fd = -1;
	this->m_stopRequest = false;
}

GpioIsr::~GpioIsr() {
	if (m_gpio_fd != -1) {
		gpioUnexport(m_gpio);
		fdClose(m_gpio_fd);
	}
	if (m_observer != nullptr) {
		delete m_observer;
	}
}

/****************************************************************
 *gpioExport
 ****************************************************************/
int GpioIsr::gpioExport(unsigned int t_gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", t_gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

/****************************************************************
 * gpioUnexport
 ****************************************************************/
int GpioIsr::gpioUnexport(unsigned int t_gpio) {
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/unexport");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", t_gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

/****************************************************************
 * setDir
 ****************************************************************/
int GpioIsr::setDir(unsigned int t_gpio, unsigned int t_out_flag) {
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf),"%s/gpio%d/direction", SYSFS_GPIO_DIR, t_gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("t_gpio/direction");
		return fd;
	}

	if (t_out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);

	close(fd);
	return 0;
}

/****************************************************************
 * setValue
 ****************************************************************/
int GpioIsr::setValue(unsigned int t_gpio, unsigned int t_value) {
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf),  "%s/gpio%d/value",SYSFS_GPIO_DIR, t_gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}

	if (t_value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}

/****************************************************************
 * getValue
 ****************************************************************/
int GpioIsr::getValue(unsigned int t_gpio, unsigned int *t_value) {
	int fd, len;
	char buf[MAX_BUF];
	char ch;

	len = snprintf(buf, sizeof(buf),"%s/gpio%d/value",SYSFS_GPIO_DIR, t_gpio);

	fd = open(buf, O_RDONLY);
	if (fd < 0) {
		perror("gpio/get-value");
		return fd;
	}

	read(fd, &ch, 1);

	if (ch != '0') {
		*t_value = 1;
	} else {
		*t_value = 0;
	}

	close(fd);
	return 0;
}

/****************************************************************
 * setEdge
 ****************************************************************/
int GpioIsr::setEdge(unsigned int t_gpio, char *t_edge) {
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf),"%s/gpio%d/edge", SYSFS_GPIO_DIR, t_gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-edge");
		return fd;
	}

	write(fd, t_edge, strlen(t_edge) + 1);
	close(fd);
	return 0;
}

/****************************************************************
 * fdOpen
 ****************************************************************/
int GpioIsr::fdOpen(unsigned int t_gpio) {
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), "%s/gpio%d/value", SYSFS_GPIO_DIR, t_gpio);

	fd = open(buf, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror("gpio/fd_open");
	}
	return fd;
}

/****************************************************************
 * fdClose
 ****************************************************************/
int GpioIsr::fdClose(int t_fd) {
	return close(t_fd);
}

int GpioIsr::start() {
	char *buf[MAX_BUF];
	int rc;
	int len;
	int val;
	gpioExport(m_gpio);
	setDir(m_gpio, 0);
	switch (m_edge) {
	case 0:
		setEdge(m_gpio, "falling");
		break;
	case 1:
		setEdge(m_gpio, "rising");
		break;
	case 2:
		setEdge(m_gpio, "both");
		break;
	default:
		setEdge(m_gpio, "rising");
		break;
	}
	m_gpio_fd = fdOpen(m_gpio);
	while (!m_stopRequest) {
		memset((void*) m_fdset, 0, sizeof(m_fdset));

		m_fdset[0].fd = m_gpio_fd;
		m_fdset[0].events = POLLPRI; // POLLIN;

		rc = poll(m_fdset, m_nfds, m_timeout);

		if (m_fdset[0].revents & POLLPRI) {
			lseek(m_fdset[0].fd, 0, SEEK_SET);
			len = read(m_fdset[0].fd, buf, MAX_BUF);
			m_observer->observe();
		}
	}
	gpioUnexport(m_gpio);
	fdClose(m_gpio_fd);
	m_gpio_fd = -1;
	m_stopRequest = false;
	return 0;
}

int GpioIsr::stop() {
	m_stopRequest = true;
	return 0;
}
