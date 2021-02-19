/*
 * mpu6050.c
 *
 *  Created on: Jan 28, 2021
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


#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int handle;
int acclX, acclY, acclZ;
int gyroX, gyroY, gyroZ;
int gyrox, gyroy, gyroz;
double acclX_scaled, acclY_scaled, acclZ_scaled;
double gyroX_scaled, gyroY_scaled, gyroZ_scaled;

int read_word_2c(int addr)
{
	int val;
	val = i2cReadByteData(handle, addr);
	val = val << 8;
	val += i2cReadByteData(handle, addr+1);
	if (val >= 0x8000)
		val = -(65536 - val);

	return val;
}

double dist(double a, double b)
{
	return sqrt((a*a) + (b*b));
}

double get_y_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(x, dist(y, z));
	return -(radians * (180.0 / M_PI));
}

double get_x_rotation(double x, double y, double z)
{
	double radians;
	radians = atan2(y, dist(x, z));
	return (radians * (180.0 / M_PI));
}

int main(int argc, char **argv) {
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	} else {
		handle = i2cOpen(1, 0x68, 0);
		if (handle < 0) {
			perror("Could not open I2C device\n");
			gpioTerminate();
			exit(1);
		}
		i2cWriteByteData(handle,0x6B,0x00);//disable sleep mode
		printf("set 0x6B=%X\n",i2cReadByteData (handle,0x6B));
		fflush(stdout);
		while(1) {
			acclX = read_word_2c(0x3B);
			acclY = read_word_2c(0x3D);
			acclZ = read_word_2c(0x3F);
			gyrox = read_word_2c(0x43);
			gyroy = read_word_2c(0x45);
			gyroz = read_word_2c(0x47);

			acclX_scaled = acclX / 16384.0;
			acclY_scaled = acclY / 16384.0;
			acclZ_scaled = acclZ / 16384.0;

			printf("X: %f Y: %f Z: %f\n", acclX_scaled, acclY_scaled, acclZ_scaled);
//			printf("X: %f Y: %f\n",get_x_rotation(acclX_scaled, acclY_scaled, acclZ_scaled), get_y_rotation(acclX_scaled, acclY_scaled, acclZ_scaled));
//			printf("gyroX: %d gyroY: %d gyroZ: %d\n",gyrox, gyroy, gyroz);
			usleep(100000);
		}
	}
	gpioTerminate();
	exit(0);
}
