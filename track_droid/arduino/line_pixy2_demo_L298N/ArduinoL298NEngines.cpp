/*
 * ArduinoL298NEngines is a porting to the L298N driver of the zumo shield.
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
#include "ArduinoL298NEngines.h"

// constructor
ArduinoL298NEngines::ArduinoL298NEngines(unsigned int leftPin1, unsigned int leftPin2, unsigned int leftPinSpeed, unsigned int rightPin1, unsigned int rightPin2, unsigned int rightPinSpeed)
{
	this->leftPin1 = leftPin1;
	this->leftPin2 = leftPin2;
	this->leftPinSpeed = leftPinSpeed;
	this->rightPin1 = rightPin1;
	this->rightPin2 = rightPin2;
	this->rightPinSpeed = rightPinSpeed;
	pinMode(leftPin1, OUTPUT);
	pinMode(leftPin2, OUTPUT);
	pinMode(leftPinSpeed, OUTPUT);
	pinMode(rightPin1, OUTPUT);
	pinMode(rightPin2, OUTPUT);
	pinMode(rightPinSpeed, OUTPUT);
	flipLeft = false;
	flipRight = false;
}

// enable/disable flipping of left motor
void ArduinoL298NEngines::flipLeftMotor(boolean flip)
{
  flipLeft = flip;
}

// enable/disable flipping of right motor
void ArduinoL298NEngines::flipRightMotor(boolean flip)
{
  flipRight = flip;
}

// set speed for left motor; speed is a number between -400 and 400
void ArduinoL298NEngines::setLeftSpeed(int speed)
{
    
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
	  digitalWrite(leftPin1, HIGH);
	  digitalWrite(leftPin2, LOW);
	  analogWrite(leftPinSpeed, speed * 51 / 80);
  }
  else
  {
	  digitalWrite(leftPin1, LOW);
	  digitalWrite(leftPin2, HIGH);
	  analogWrite(leftPinSpeed, speed * 51 / 80);
  }
}

// set speed for right motor; speed is a number between -400 and 400
void ArduinoL298NEngines::setRightSpeed(int speed)
{
    
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
	  digitalWrite(rightPin1, HIGH);
	  digitalWrite(rightPin2, LOW);
	  analogWrite(rightPinSpeed, speed * 51 / 80);
  }
  else
  {
	digitalWrite(rightPin1, LOW);
	digitalWrite(rightPin2, HIGH);
	analogWrite(rightPinSpeed, speed * 51 / 80);
  }
}

// set speed for both motors
void ArduinoL298NEngines::setSpeeds(int leftSpeed, int rightSpeed)
{
  setLeftSpeed(leftSpeed);
  setRightSpeed(rightSpeed);
}