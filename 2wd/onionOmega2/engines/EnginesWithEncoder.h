/*
 Engine driver with encoder.
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
#include "Engines.h"
#include "EncoderObserver.h"
#include <GpioIsr.h>
#include <ThreadExecutors.h>

#ifndef EnginesWithEncoder_h
#define EnginesWithEncoder_h

class EnginesWithEncoder: public Engines {
public:
	EnginesWithEncoder(float t_whellRadius, long t_resolutionCoderLeft,
			long t_resolutionCoderRight, long t_countRotate1Inner,
			long t_countRotate1Outer, unsigned int t_leftEncoderPin,
			unsigned int t_rightEncoderPin);
	EnginesWithEncoder();
	virtual ~EnginesWithEncoder();
	//encoders operations
	long getLeftCounter();
	long getRightCounter();
	void resetCounters();
	void startIsr();
	void stopIsr();
	//power operations
	void setLeftEnginePower(unsigned int t_power);
	unsigned int getLeftEnginePower();
	void setRightEnginePower(unsigned int t_power);
	unsigned int getRightEnginePower();
	int getIdlePower();
	void setIdlePower(unsigned int t_power);
	virtual void increasePower(int t_speadIncrese);
	//pure virtual those depends on the engine driver
	virtual void go(int t_leftSpeed, int t_rightSpeed) = 0;
	virtual void stopLeftEngine() = 0;
	virtual void stopRightEngine() = 0;
	//movement
	void fullStop();
	void rotateDegree(int t_degree);
	void linearMove(float t_distance);
	float getLeftDistance();
	float getRightDistance();
private:
	float m_whellRadius;
	long m_resolutionEncoderLeft;
	long m_resolutionEncoderRight;
	long m_countRotate1Inner;
	long m_countRotate1Outer;
	float m_PPI_left;
	float m_PPI_right;
	volatile long m_left_encoder_A_count;
	volatile long m_right_encoder_A_count;
	unsigned int m_leftEncoderIsrPin;
	unsigned int m_rightEncoderIsrPin;
	unsigned int m_leftEnginePower;
	unsigned int m_rightEnginePower;
	unsigned int m_idlePower;
	const std::string LEFT_ENGINE = "left";
	const std::string RIGHT_ENGINE = "right";
	void rotateLeftDegree(int t_degree);
	void rotateRightDegree(int t_degree);
	void forwardMove(float t_distance);
	void backwardMove(float t_distance);
	ThreadExecutors *m_executors;
	volatile float m_currentLeftPosition;
	volatile float m_currentRightPosition;
};
#endif
