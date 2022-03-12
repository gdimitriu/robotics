/*
 * CDroid.h
 *
 *  Created on: Oct 8, 2020
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

#ifndef DROID_CDROID_H_
#define DROID_CDROID_H_

#include <CMasterControlEngines.h>
#include <CMasterControlSensors.h>
#include <CSettingLoading.h>
#include <Adafruit_PWMServoDriver.h>
#include <iostream>
#include <fstream>
#include <CGenericSensor.h>
#include <list>
#include <CResetableBarrier.h>
#include <pthread.h>
#include <CGrabberController.h>
#include <CCamera.h>
#include <string>
class CDroid {
public:
	CDroid(char *droidCfgFile, int isOnHost);
	virtual ~CDroid();
//	virtual void initialize(std::ifstream *pFile);
	virtual void dumpInfo();
	/* rotate left(-1)/nothing(0)/right(1) and then move distance */
	virtual void move(float distance, int rotation, int check);
	virtual void move(int direction, int rotation);
	virtual void fullStop();
	CLogger* getLogger();
	/*
	 * set the maximum engine power
	 */
	void setMaxEnginePower(unsigned int newPower);
	/*
	 * set the minimum engine power
	 */
	void setMinEnginePower(unsigned int newPower);
	/*
	 * get the maximum engine power
	*/
	unsigned int getMaxEnginePower();
	/*
	 * get the minimum engine power
	*/
	unsigned int getMinEnginePower();
	/*
	 * set the current engine power
	 */
	void setCurrentEnginePower(unsigned int newPower);
	/*
	 * get the current engine power
	 */
	unsigned int getCurrentEnginePower();
	/*
	 * get the low power distance
	*/
	unsigned long getLowPowerDistance();
	/*
	 * set the low power distance
	*/
	void setLowPowerDistance(unsigned long distance);
	/*
	 * set the stop distance
	 */
	void setStopDistance(unsigned int distance);
	/*
	 * get the stop distance
	 */
	unsigned int getStopDistance();
	/*
	 * set blocking on nonblocking operation
	 */
	void setBlockingOperation(bool type);

	void captureCameraImage(std::ofstream *pFile);

	void startStreaming();
	void stopStreaming();
	/*
	 * grab the object that is already in claw using 1/2 claw
	 */
	void openClaw();
	void closeClaw();
	std::string* getInfo();
protected:
	/*
	 * nrEncoder + is right and - is left
	 * check is 0 for normal rotate and 1 for rotate with checking distance and then restore safe distance
	 * (it will rotate until it will find a distance > stopDistance)
	 */
	virtual void rotate(int nrEncoder, int check);
	virtual void initialize();
	static void mainCollisionCallback(void *instance, CGenericSensor *sensor);
	friend void* CDroid_InternalMoveThread(void *droidInstance);
	virtual void internalMove();
	virtual void internalMoveLeft();
	virtual void internalMoveRight();
	virtual void move(float distance);
	virtual void forwardMove(float newDistance, unsigned int newPower);
	virtual void backwardMove(float newDistance, unsigned int newPower);
	char* getLine();
	void collisionCallback(CGenericSensor *sensor);
	CMasterControlEngines *m_controlEngines;
	CMasterControlSensors *m_controlSensors;
	CLogger *m_logger;
	std::ifstream *m_pFile;
	char *m_buffer;
	unsigned int m_buffSize;
	Adafruit_PWMServoDriver *m_pwmDriver;
	int m_isOnHost;
	unsigned int m_stopDistance;
	std::list<CGenericSensor *> m_triggeredSensors;
	char *m_droidCfgFile;
	int m_maxLeftServoEncoder;
	int m_maxRightServoEncoder;
	CResetableBarrier m_moveBarrier;
	float m_nextDistance;
	int m_nextRotation;
	int m_nextCheckRotate;
	pthread_t m_moveThread;
	pthread_attr_t m_moveThreadAttr;
	float m_actualDistance;
	CGrabberController *m_grabberController;
	CLogger* m_settingLogger;
	bool m_blockingOperation;
	CCamera* m_camera;
};

#endif /* DROID_CDROID_H_ */
