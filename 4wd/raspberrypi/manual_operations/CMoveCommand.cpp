/*
 * CMoveCommand.cpp
 *
 *  Created on: Oct 5, 2020
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

#include "CMoveCommand.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CMoveCommand::CMoveCommand() {
	init();
}

CMoveCommand::CMoveCommand(CLogger* logger) : CCommand(logger) {
	init();
}

void CMoveCommand::init() {
	m_operations = new set<char>();

	m_menu = new string();
	m_operations->insert('b');
	m_operations->insert('m');
	m_operations->insert('l');
	m_operations->insert('r');
	m_operations->insert('M');
	m_menu->append("b# (break) full stop\t;\n");
	m_menu->append("mxxx,yyy# move (forward xxx > 0  xxx = 0 rotate or xxx < 0 backward) cm and rotate before \n");
	m_menu->append("(-1 left, 0 center, 1 right) or (negative for left encoder and positive for right encoder)\t;\n");
	m_menu->append("l# rotate left 45 degree and restore safe distance\t;\n");
	m_menu->append(
		"lxxx# rotate left in step of 45 degree where is distance and move xxx cm\t;\n");
	m_menu->append("r# rotate right 45 degree and restore safe distance\t;\n");
	m_menu->append(
			"rxxx# rotate right in step of 45 degree where is distance and move xxx cm\t;\n");
	m_menu->append("Mxxx,yyy# move or rotate until another command\t;\n");
	m_droid = NULL;
}

CMoveCommand::~CMoveCommand() {
	m_operations->clear();
	delete m_operations;
	delete m_menu;
}

void CMoveCommand::setDroid(CDroid *droid) {
	m_droid = droid;
	if (m_logger == NULL) {
		m_logger = droid->getLogger();
	}
}

set<char>* CMoveCommand::getOperationsPrefix() {
	return m_operations;
}

void CMoveCommand::printMenu() {
	m_logger->info(*m_menu);
}

int CMoveCommand::executeSimpleCommand(char *operation) {
	switch (operation[0]) {
	case 'b':
		m_logger->debug("Stop all engines !\n");
		m_droid->fullStop();
		break;
	case 'l':
		switch (m_reverseType) {
		case 1:
			m_logger->debug("Rotate right 45 degree !\n");
			m_droid->move(0, 1, 0);
			break;
		case 2:
			m_logger->debug("Rotate right 45 degree !\n");
			m_droid->move(0, 1, 0);
			break;
		case 0://do nothing
		default:
			m_logger->debug("Rotate left 45 degree !\n");
			m_droid->move(0, -1, 0);
		}
		break;
	case 'r':
		switch (m_reverseType) {
		case 1:
			m_logger->debug("Rotate left 45 degree !\n");
			m_droid->move(0, -1, 0);
			break;
		case 2:
			m_logger->debug("Rotate left 45 degree !\n");
			m_droid->move(0, -1, 0);
			break;
		case 0://do nothing
		default:
			m_logger->debug("Rotate right 45 degree !\n");
			m_droid->move(0, 1, 0);
		}
		break;
	default:
		char message[255];
		sprintf(message,"Invalid move with simple command = %s\n", operation);
		m_logger->error(message);
		return 1;
	}
	return 0;
}

int CMoveCommand::executeDataCommand(char *operation) {
	float distance;
	int rotation;
	int direction;
	char message[REPLY_MESSAGE_LENGTH];
	memset(message, 0, REPLY_MESSAGE_LENGTH * sizeof(char));
	switch (operation[0]) {
	case 'm':
		removeCommandPrefix(operation);
		sscanf(operation, "%f,%d", &distance, &rotation);
		switch (m_reverseType) {
		case 1:
			rotation = -rotation;
			break;
		case 2:
			rotation = -rotation;
			distance = -distance;
			break;
		case 0://do nothing
		default:
			break;
		}
		if (m_logger->isDebug()) {
			if (rotation == 0) {
				if (distance > 0) {
					sprintf(message, "Move forward with %f cm\n", distance);
				} else if (distance < 0) {
					sprintf(message, "Move backward with %f cm\n", distance);
				}
			}
			m_logger->debug(message);
		}
		m_droid->move(distance, rotation, 0);
		break;
	case 'l':
		removeCommandPrefix(operation);
		if (isValidNumber(operation) == 0) {
			sprintf(message,"Invalid command = %s \n", operation);
			m_logger->error(message);
			return 0;
		}
		distance = atof(operation);
		sprintf(message, "Move left after find space with %f cm\n", distance);
		m_logger->debug(message);
		m_droid->move(distance, -1, 1);
		break;
	case 'r':
		removeCommandPrefix(operation);
		if (isValidNumber(operation) == 0) {
			sprintf(message,"Invalid command = %s \n", operation);
			m_logger->error(message);
			return 0;
		}
		distance = atof(operation);
		sprintf(message, "Move right after find space with %f cm\n", distance);
		m_logger->debug(message);
		m_droid->move(distance, 1, 1);
		break;
	case 'M':
		removeCommandPrefix(operation);
		sscanf(operation, "%d,%d", &direction, &rotation);
		if (m_logger->isDebug()) {
			if (rotation == 0 && direction == 0) {
				sprintf(message, "Coast all engines\n");
			} else	if (rotation == 0) {
				if (direction > 0) {
					sprintf(message, "Move forward\n");
				} else if (direction < 0) {
					sprintf(message, "Move backward\n");
				}
			} else if (direction == 0) {
				if (rotation > 0) {
					sprintf(message, "Rotate right\n");
				} else if (rotation < 0) {
					sprintf(message, "Rotate left\n");
				}
			}
			m_logger->debug(message);
		}
		m_droid->move(direction, rotation);
		break;
	default:
		sprintf(message,"Invalid move with data command = %s\n", operation);
		m_logger->error(message);
		return 1;
	}
	return 0;
}

