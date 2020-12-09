/*
ArduinoL298NEngines is a porting to the L298N driver of the zumo shield.
Copyright 2019 Gabriel Dimitriu

ArduinoL298NEngines is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
ArduinoL298NEngines is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Autonomous Droid; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#ifndef ArduinoL298NEngines_h
#define ArduinoL298NEngines_h

#include <Arduino.h>

class ArduinoL298NEngines
{
  public:  
  
    // constructor
	 
	ArduinoL298NEngines(unsigned int leftPin1, unsigned int leftPin2, unsigned int leftPinSpeed, unsigned int rightPin1, unsigned int rightPin2, unsigned int rightPinSpeed);
    
    // enable/disable flipping of motors
    void flipLeftMotor(boolean flip);
    void flipRightMotor(boolean flip);
    
    // set speed for left, right, or both motors
    void setLeftSpeed(int speed);
    void setRightSpeed(int speed);
    void setSpeeds(int leftSpeed, int rightSpeed);
    
  private:
	  boolean flipLeft;
	  boolean flipRight;
	  unsigned int leftPin1;
	  unsigned int leftPin2;
	  unsigned int leftPinSpeed;
	  unsigned int rightPin1;
	  unsigned int rightPin2;
	  unsigned int rightPinSpeed;
};

#endif