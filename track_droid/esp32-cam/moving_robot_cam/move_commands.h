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

#ifndef __MOVE_COMMANDS_H__
#define __MOVE_COMMANDS_H__

#include "configuration.h"

bool moveOrRoatateWithDistanceCommand(char *realData);

bool isValidNumber( char *data, int size );

bool setMinPowerCommand(char *realData);

bool setMaxPowerCommand(char *realData);

bool setCurrentPowerCommand(char *realData);

bool moveOrRotateUntilNextCommand(char *realData);

void setupEngines();

void go( int speedLeft, int speedRight );
#endif
