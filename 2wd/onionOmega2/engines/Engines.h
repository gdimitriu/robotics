/*
Engine driver for droid
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

class Engines
{
public:
	virtual long getLeftCounter() = 0;
	virtual long getRightCounter() = 0;
	virtual void resetCounters() = 0;
	virtual void go(int leftSpeed, int rightSpeed) = 0;
	virtual void rotateDegree(int degree) = 0;
	virtual void linearMove(float distance) = 0;
	virtual void fullStop() = 0;
	virtual ~Engines();
};
