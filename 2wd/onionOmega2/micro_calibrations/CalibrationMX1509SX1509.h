/*
Calibrations with MX1509 and extender SX1509
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

#ifndef CALIBRATION_MX1509_SX1509_H_
#define CALIBRATION_MX1509_SX1509_H_

#include <EnginesWithEncoderMX1509SX1509.h>

class CalibrationMX1509SX1509 {
public:
	CalibrationMX1509SX1509();
	virtual ~CalibrationMX1509SX1509();
	void start();
private:
	void printMenu();
	EnginesWithEncoderMX1509SX1509 * m_engines;
};

#endif /* CALIBRATION_MX1509_SX1509_H_ */
