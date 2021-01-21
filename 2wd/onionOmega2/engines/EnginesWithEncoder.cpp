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
#include "EnginesWithEncoder.h"
#include <iostream>

EnginesWithEncoder::EnginesWithEncoder(float t_whellRadius,
		long t_resolutionCoderLeft, long t_resolutionCoderRight,
		long t_countRotate1Inner, long t_countRotate1Outer,
		unsigned int t_leftEncoderPin, unsigned int t_rightEncoderPin) {
	float pi = 3.14f;
	this->m_whellRadius = t_whellRadius;
	this->m_resolutionEncoderLeft = t_resolutionCoderLeft;
	this->m_resolutionEncoderRight = t_resolutionCoderRight;
	this->m_countRotate1Inner = t_countRotate1Inner;
	this->m_countRotate1Outer = t_countRotate1Outer;
	this->m_PPI_left = m_resolutionEncoderLeft / (2 * pi * this->m_whellRadius);
	this->m_PPI_right = m_resolutionEncoderRight / (2 * pi * this->m_whellRadius);
	this->m_leftEncoderIsrPin = t_leftEncoderPin;
	this->m_rightEncoderIsrPin = t_rightEncoderPin;
	this->m_leftEnginePower = 255;
	this->m_rightEnginePower = 255;
	this->m_idlePower = 0;
	//executors
	m_executors = new ThreadExecutors();
	//observer and isr for left engine
	EncoderObserver *observerEncoder = new EncoderObserver(
			&m_left_encoder_A_count, LEFT_ENGINE);
	GpioIsr *encoderIsr = new GpioIsr(m_leftEncoderIsrPin, 1, observerEncoder,
			1000);
	m_executors->addExecutor(LEFT_ENGINE, encoderIsr);
	//observer and isr for right engine
	observerEncoder = new EncoderObserver(&m_right_encoder_A_count, RIGHT_ENGINE);
	encoderIsr = new GpioIsr(m_rightEncoderIsrPin, 1, observerEncoder, 1000);
	m_executors->addExecutor(RIGHT_ENGINE, encoderIsr);
}

EnginesWithEncoder::~EnginesWithEncoder() {
	if (m_executors != nullptr) {
		delete m_executors;
	}
}
EnginesWithEncoder::EnginesWithEncoder() {
	this->m_whellRadius = 0;
	this->m_resolutionEncoderLeft = 0;
	this->m_resolutionEncoderRight = 0;
	this->m_countRotate1Inner = 0;
	this->m_countRotate1Outer = 0;
	this->m_PPI_left = 0;
	this->m_PPI_right = 0;
	this->m_leftEncoderIsrPin = 0;
	this->m_rightEncoderIsrPin = 0;
	this->m_leftEnginePower = 255;
	this->m_rightEnginePower = 255;
	this->m_idlePower = 0;
	//executors
	m_executors = nullptr;
	m_currentLeftPosition = 0;
	m_currentRightPosition = 0;
}

long EnginesWithEncoder::getLeftCounter() {
	return m_left_encoder_A_count;
}

long EnginesWithEncoder::getRightCounter() {
	return m_right_encoder_A_count;
}

void EnginesWithEncoder::resetCounters() {
	m_left_encoder_A_count = 0;
	m_right_encoder_A_count = 0;
}

void EnginesWithEncoder::startIsr() {
	resetCounters();
	m_executors->startExecutor(LEFT_ENGINE);
	m_executors->startExecutor(RIGHT_ENGINE);
}

void EnginesWithEncoder::stopIsr() {
	m_executors->stopExecutor(LEFT_ENGINE);
	m_executors->stopExecutor(RIGHT_ENGINE);
}

void EnginesWithEncoder::fullStop() {
	stopRightEngine();
	stopLeftEngine();
	stopIsr();
}

void EnginesWithEncoder::rotateDegree(int t_degree) {
	startIsr();
	if (t_degree < 0) {
		rotateLeftDegree(-t_degree);
	} else if (t_degree > 0) {
		rotateRightDegree(t_degree);
	}
	stopIsr();
}

void EnginesWithEncoder::linearMove(float t_distance) {
	startIsr();
	if (t_distance < 0) {
		backwardMove(-t_distance);
	} else if (t_distance > 0) {
		forwardMove(t_distance);
	}
	stopIsr();
}

void EnginesWithEncoder::rotateLeftDegree(int t_degree) {
	go(-m_leftEnginePower, m_rightEnginePower);
	while (m_left_encoder_A_count < m_countRotate1Inner * t_degree
			&& m_right_encoder_A_count < m_countRotate1Outer * t_degree)
		;
	go(0, 0);
}

void EnginesWithEncoder::rotateRightDegree(int t_degree) {
	go(m_leftEnginePower, -m_rightEnginePower);
	while (m_left_encoder_A_count < m_countRotate1Outer * t_degree
			&& m_right_encoder_A_count < m_countRotate1Inner * t_degree)
		;
	go(0, 0);
}

void EnginesWithEncoder::forwardMove(float t_distance) {
	go(m_leftEnginePower, m_rightEnginePower);
	bool stopLeft = false;
	bool stopRight = false;
	m_currentLeftPosition = 0.0;
	m_currentRightPosition = 0.0;
	while (!(stopLeft && stopRight)) {
		if (!stopLeft) {
			if ((t_distance - m_currentLeftPosition) > 0.02) {
				m_currentLeftPosition = getLeftDistance();
			} else {
				stopLeftEngine();
				std::cout<<"stop left engine "<<m_currentLeftPosition<<" count="<<m_left_encoder_A_count<<" ppi="<<m_PPI_left<<std::endl;
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if ((t_distance - m_currentRightPosition) > 0.02) {
				m_currentRightPosition = getRightDistance();
			} else {
				stopRightEngine();
				std::cout<<"stop right engine "<<m_currentRightPosition<<" count="<<m_right_encoder_A_count<<" ppi="<<m_PPI_right<<std::endl;
				stopRight = true;
			}
		}
	}
	go(0, 0);
	m_currentLeftPosition = 0;
	m_currentRightPosition = 0;
}

void EnginesWithEncoder::backwardMove(float t_distance) {
	go(-m_leftEnginePower, -m_rightEnginePower);
	bool stopLeft = false;
	bool stopRight = false;
	while (!(stopLeft && stopRight)) {
		if (!stopLeft) {
			if ((t_distance - m_currentLeftPosition) > 0.02) {
				m_currentLeftPosition = getLeftDistance();
			} else {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if ((t_distance - m_currentRightPosition) > 0.02) {
				m_currentRightPosition = getRightDistance();
			} else {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0, 0);
	m_currentLeftPosition = 0;
	m_currentRightPosition = 0;
}

float EnginesWithEncoder::getLeftDistance() {
	return m_left_encoder_A_count / m_PPI_left;
}

float EnginesWithEncoder::getRightDistance() {
	return m_right_encoder_A_count / m_PPI_right;
}

void EnginesWithEncoder::setLeftEnginePower(unsigned int t_power) {
	this->m_leftEnginePower = t_power;
}

unsigned int EnginesWithEncoder::getLeftEnginePower() {
	return this->m_leftEnginePower;
}

void EnginesWithEncoder::setRightEnginePower(unsigned int t_power) {
	this->m_rightEnginePower = t_power;
}

unsigned int EnginesWithEncoder::getRightEnginePower() {
	return this->m_rightEnginePower;
}

int EnginesWithEncoder::getIdlePower() {
	return this->m_idlePower;
}

void EnginesWithEncoder::setIdlePower(unsigned int t_power) {
	this->m_idlePower = t_power;
}

void EnginesWithEncoder::increasePower(int t_speadIncrese) {

}
