/*
 * configuration.c
 *
 *  Created on: June 29, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

	#define SERIAL_DEBUG_MODE true
	
	#define HIGH 1
	#define LOW 0

	extern int bufferIndex;
	extern int absoluteMaxPower;
	extern int maxPower;
	extern int currentPower;
	extern int minPower;
	/*
	 * engine settings
	 */
	extern float whellRadius;
	extern const float pi;
	//left engine
	extern unsigned int leftMotorPin1;
	extern unsigned int leftMotorPin2;
	extern unsigned int leftMotorEncoder;
	extern long leftResolutionCodor;
	extern float leftPPI;
	//right engine
	extern unsigned int rightMotorPin1;
	extern unsigned int rightMotorPin2;
	extern unsigned int rightMotorEncoder;
	extern long rightResolutionCodor;
	extern float rightPPI;
	extern long countRotate90Left;
	extern long countRotate90Right;
	//sensors
	extern unsigned int frontSensorPin;
	extern unsigned int backSensorPin;
#endif
