/*
 * cameraBle.cpp
 *
 * Raspberry Camera take a picture on c# command from ble
 *  Created on: Dec 30, 2020
 *      Author: Gabriel Dimitriu
 *
 * You have to link the /dev/rfcomm0 to /dev/serialx and use as argument after connect to the ble.
 * You have also to run before connecting to ble : /usr/bin/rfcomm watch hdi0
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

#include <fstream>
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raspicam.h>
#include <ctime>
#include <unistd.h>
#include <iostream>
#include <raspicam_still.h>
using namespace std;

int main(int argc, char **argv) {

	if (argc != 2) {
		fprintf(stderr,"Usage: %s ttynr\n", argv[0]);
		exit(1);
	}
	if (gpioInitialise() < 0) {
		perror("gpio init failed");
		exit(1);
	}
	int serialHandler = serOpen(argv[1], 38400, 0);
	char sendBuffer[255];
	if (serialHandler < 0) {
		perror("gpio serial open failed");
		gpioTerminate();
		exit(1);
	}
	raspicam::RaspiCam_Still Camera; //Camera object
		//Open camera
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer, "Opening Camera...\n");
		serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
		if ( !Camera.open())
		{
			memset(sendBuffer, 0, sizeof(sendBuffer));
			sprintf(sendBuffer, "Error opening camera\n");
			serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
			gpioTerminate();
			return -1;
		}
		//wait a while until camera stabilizes
		memset(sendBuffer, 0, sizeof(sendBuffer));
		sprintf(sendBuffer, "Sleeping for 3 secs\n");
		serWrite(serialHandler, sendBuffer, strlen(sendBuffer));
		sleep(3);
	char buffer[255];

	do {
		if (serDataAvailable(serialHandler) > 0) {
			memset(buffer, 0, sizeof(buffer));
			int readnr = serRead(serialHandler, buffer, 255);
			for(int i = 0; i < readnr; i++) {
				if (buffer[i] == '\n' || buffer[i] == '\r') {
					buffer[i] = '\0';
				}
			}
			if (strcmp(buffer,"exit#") == 0 ) {
				break;
			}
			char * newOperation = (char *)calloc(strlen(buffer) +1, sizeof(char));
			strcpy(newOperation, buffer);
			//clear the #
			newOperation[strlen(newOperation) -1] = '\0';
			if (newOperation[0] == 'c') {
				Camera.setWidth ( 800);//2592 );
				Camera.setHeight ( 600);//1944 );
				Camera.setISO(400);
				Camera.setQuality(10);
				Camera.setEncoding ( raspicam::RASPICAM_ENCODING_JPEG );
				unsigned int length = Camera.getImageBufferSize(); // Header + Image Data + Padding
				unsigned char * data = new unsigned char[length];
				      if ( !Camera.grab_retrieve(data, length) ) {
				        std::cerr<<"Error in grab"<<std::endl;
				        return -1;
				    }

				    std::cout<<"saving picture.jpg"<<std::endl;
				    ofstream file ( "picture.jpg",ios::binary );
				    file.write ( ( char* ) data,   length );
				delete data;
			}
		}
	} while(strcmp(buffer,"exit#") != 0);
}
