/*
 Engine driver with MX1508
 Copyright (C) 2020 Gabriel Dimitriu
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
#include "EnginesWithEncoderMX1508.h"

EnginesWithEncoderMX1508::EnginesWithEncoderMX1508(float t_whellRadius,
		long t_resolutionCoderLeft, long t_resolutionCoderRight,
		long t_countRotate1Inner, long t_countRotate1Outer,
		unsigned int t_leftEncoderPin, unsigned int t_rightEncoderPin) :
		EnginesWithEncoder(t_whellRadius, t_resolutionCoderLeft,
				t_resolutionCoderRight, t_countRotate1Inner, t_countRotate1Outer,
				t_leftEncoderPin, t_rightEncoderPin) {
	this->m_leftEnginePin1 = 0;
	this->m_leftEnginePin2 = 0;
	this->m_rightEnginePin1 = 0;
	this->m_rightEnginePin2 = 0;
}

EnginesWithEncoderMX1508::EnginesWithEncoderMX1508() {
	this->m_leftEnginePin1 = 0;
	this->m_leftEnginePin2 = 0;
	this->m_rightEnginePin1 = 0;
	this->m_rightEnginePin2 = 0;
}

EnginesWithEncoderMX1508::~EnginesWithEncoderMX1508() {
	// TODO Auto-generated destructor stub
}

void EnginesWithEncoderMX1508::setEnginesPins(unsigned char t_leftPin1,
		unsigned char t_leftPin2, unsigned char t_rightPin1,
		unsigned char t_rightPin2) {
	this->m_leftEnginePin1 = t_leftPin1;
	this->m_leftEnginePin2 = t_leftPin2;
	this->m_rightEnginePin1 = t_rightPin1;
	this->m_rightEnginePin2 = t_rightPin2;
}

unsigned char EnginesWithEncoderMX1508::getLeftEnginePin1() {
	return m_leftEnginePin1;
}

unsigned char EnginesWithEncoderMX1508::getLeftEnginePin2() {
	return m_leftEnginePin2;
}
unsigned char EnginesWithEncoderMX1508::getRightEnginePin1() {
	return m_rightEnginePin1;
}

unsigned char EnginesWithEncoderMX1508::getRightEnginePin2() {
	return m_rightEnginePin2;
}
