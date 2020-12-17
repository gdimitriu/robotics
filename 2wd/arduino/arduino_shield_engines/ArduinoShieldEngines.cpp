/*
 * ArduinoShieldEngines is a porting to the arduino shield of the zumo shield.
 * Copyright 2019 Gabriel Dimitriu

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
#include "ArduinoShieldEngines.h"

#define LEFT_MOTOR_PIN1 3
#define LEFT_MOTOR_PIN2 5
#define RIGHT_MOTOR_PIN1 6
#define RIGHT_MOTOR_PIN2 9

static boolean flipLeft = false;
static boolean flipRight = false;

// constructor (doesn't do anything)
ArduinoShieldEngines::ArduinoShieldEngines()
{
}

// initialize timer1 to generate the proper PWM outputs to the motor drivers
void ArduinoShieldEngines::init2()
{
  pinMode(LEFT_MOTOR_PIN1,  OUTPUT);
  pinMode(LEFT_MOTOR_PIN2,  OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
}

// enable/disable flipping of left motor
void ArduinoShieldEngines::flipLeftMotor(boolean flip)
{
  flipLeft = flip;
}

// enable/disable flipping of right motor
void ArduinoShieldEngines::flipRightMotor(boolean flip)
{
  flipRight = flip;
}

// set speed for left motor; speed is a number between -400 and 400
void ArduinoShieldEngines::setLeftSpeed(int speed)
{
  init(); // initialize if necessary
    
  boolean reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed; // make speed a positive quantity
    reverse = 1;    // preserve the direction
  }
  if (speed > 400)  // Max 
    speed = 400;
    
  if (reverse ^ flipLeft) // flip if speed was negative or flipLeft setting is active, but not both
  {
	  analogWrite(LEFT_MOTOR_PIN1, 0);
	  analogWrite(LEFT_MOTOR_PIN2, speed * 51 / 80);
  }
  else
  {
	  analogWrite(LEFT_MOTOR_PIN1, speed * 51 / 80);
	  analogWrite(LEFT_MOTOR_PIN2, 0);
  }
}

// set speed for right motor; speed is a number between -400 and 400
void ArduinoShieldEngines::setRightSpeed(int speed)
{
  init(); // initialize if necessary
    
  boolean reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;

  if (reverse ^ flipRight) // flip if speed was negative or flipRight setting is active, but not both
  {
	  analogWrite(RIGHT_MOTOR_PIN1, 0);
	  analogWrite(RIGHT_MOTOR_PIN2, speed * 51 / 80);
  }
  else
  {
	analogWrite(RIGHT_MOTOR_PIN1, speed * 51 / 80);
	analogWrite(RIGHT_MOTOR_PIN2, 0);
  }
}

// set speed for both motors
void ArduinoShieldEngines::setSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}