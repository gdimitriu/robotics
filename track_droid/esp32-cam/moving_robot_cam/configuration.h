/*
 * moving robot
 * Copyright 2024 Gabriel Dimitriu
 *
 * This file is part of robotics project.

 * robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with robotics; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <WiFi.h>

#define SERIAL_BUFFER 50

#define ABSOLUTE_MAX_POWER 65550

#define SERVER_PORT 4242

/**********************************************
 * 
 * right motor pin 2
 * this is the pin from
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN2 12

/**********************************************
 * 
 * right motor pin 1
 * this is the pin from 
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN1 13

/**********************************************
 * 
 * left motor pin 2
 * this is the pin from 
 * 
 **********************************************/
#define LEFT_MOTOR_PIN2 14

/**********************************************
 * 
 * left motor pin 1
 * this is the pin from 
 * 
 **********************************************/
#define LEFT_MOTOR_PIN1 15

extern int currentPower;

extern int minPower;

extern int maxPower;

extern int indexReceive;

extern bool isValidInput;

void makeCleanup();

void sendOK();

#endif
