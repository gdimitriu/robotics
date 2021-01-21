/*
 Engine driver with MX1509
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

#ifndef ENGINES_ENGINESWITHENCODERMX1509_H_
#define ENGINES_ENGINESWITHENCODERMX1509_H_

#include "EnginesWithEncoder.h"

class EnginesWithEncoderMX1509: public EnginesWithEncoder {
public:
	EnginesWithEncoderMX1509();
	EnginesWithEncoderMX1509(float t_whellRadius, long t_resolutionCoderLeft,
			long t_resolutionCoderRight, long t_countRotate1Inner,
			long t_countRotate1Outer, unsigned int t_leftEncoderPin,
			unsigned int t_rightEncoderPin);
	void setEnginesPins(unsigned char t_leftPin1, unsigned char t_leftPin2,
			unsigned char t_rightPin1, unsigned char t_rightPin2);
	virtual ~EnginesWithEncoderMX1509();
	virtual void enableEnginesPins() = 0; //depends of expander and command driver
protected:
	unsigned char getLeftEnginePin1();
	unsigned char getLeftEnginePin2();
	unsigned char getRightEnginePin1();
	unsigned char getRightEnginePin2();
private:
	unsigned char m_leftEnginePin1;
	unsigned char m_leftEnginePin2;
	unsigned char m_rightEnginePin1;
	unsigned char m_rightEnginePin2;
};

#endif /* ENGINES_ENGINESWITHENCODERMX1509_H_ */
