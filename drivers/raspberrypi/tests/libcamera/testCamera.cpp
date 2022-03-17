/*
 * testCamera.cpp
 *
 *  Created on: Mar 13, 2022
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
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
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char ** argv;

int main() {
	argv = (char **)calloc(12,sizeof(char *));
	argv[0] = "libcamera-vid";
	argv[1] = "-n";
	argv[2]= "-t";
	argv[3] ="0";
	argv[4] = "--codec";
	argv[5]= "mjpeg";
	argv[6] = "--framerate";
	argv[7] = "2";
	argv[8] = "--listen";
	argv[9] = "-o";
	argv[10]= "tcp://0.0.0.0:8180";
	argv[11] = NULL;
	pid_t pid =fork();
	if (pid > 0) {
		waitpid(pid,NULL,0);
		exit(1);
	}
	if (pid == 0)
		execvp(argv[0],argv);
}


