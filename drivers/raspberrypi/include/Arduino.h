/*
Arduino compability header for linux devices.
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
#ifndef ARDUINO_H
#define ARDUINO_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <time.h>
#include <stdint.h>

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define PI M_PI
#define HALF_PI M_PI_2
#define TWO_PI M_PI * 2.0
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#define sq(value) (value*value)
#define degrees(angleInDegrees) ((angleInDegrees) * PI / 180.0)
#define radians(angleInRadians) ((angleInRadians) * 180.0 / PI)

typedef unsigned char byte;
typedef bool boolean;

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define delay(value) (usleep(1000*value))

#if defined(__NEED_int16_t) && !defined(__DEFINED_int16_t)
	typedef short  int16_t;
	#define __DEFINED_int16_t
#endif

#define BUFFER_LENGTH 32

#endif
