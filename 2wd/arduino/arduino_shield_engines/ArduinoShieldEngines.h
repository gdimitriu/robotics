/*
ArduinoShieldEngines is a porting to the arduino shield of the zumo shield.
Copyright 2019 Gabriel Dimitriu

ArduinoShieldEngines is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
ArduinoShieldEngines is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Autonomous Droid; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#ifndef ArduinoShieldEngines_h
#define ArduinoShieldEngines_h

#include <Arduino.h>

class ArduinoShieldEngines
{
  public:  
  
    // constructor (doesn't do anything)
	 
	ArduinoShieldEngines();
    
    // enable/disable flipping of motors
    static void flipLeftMotor(boolean flip);
    static void flipRightMotor(boolean flip);
    
    // set speed for left, right, or both motors
    static void setLeftSpeed(int speed);
    static void setRightSpeed(int speed);
    static void setSpeeds(int leftSpeed, int rightSpeed);
    
  private:

    static inline void init()
    {
      static boolean initialized = false;

      if (!initialized)
      {
        initialized = true;
        init2();
      }
    }
    
    // initializes timer1 for proper PWM generation
    static void init2();
};

#endif