/*
 * 2enginesmove.c
 *
 *  Created on: June 29, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#include <math.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/printf.h>
#include "2enginesmove.h"
#include "configuration.h"

static volatile unsigned long leftCounts;
static volatile float leftCurrentDistance = 0.0f; 
static volatile unsigned long rightCounts;
static volatile float rightCurrentDistance = 0.0f;

unsigned long getLeftEngineCounts() {
	return leftCounts;
}

unsigned long getRightEngineCounts() {
	return rightCounts;
}

void gpio_callback_encoder(uint gpio, uint32_t events) {
    if (GPIO_IRQ_EDGE_RISE == events) {
    	if (gpio == leftMotorEncoder) {
			leftCounts++;
		} else if (gpio == rightMotorEncoder) {
			rightCounts++;
		}
    }
}

void breakEngines() {
	pwm_set_gpio_level(leftMotorPin1,absoluteMaxPower);
    pwm_set_gpio_level(leftMotorPin2,absoluteMaxPower);
    pwm_set_gpio_level(rightMotorPin1,absoluteMaxPower);
    pwm_set_gpio_level(rightMotorPin2,absoluteMaxPower);
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
	if (speedLeft == 0 && speedRight == 0 ) {
		pwm_set_gpio_level(leftMotorPin1,LOW);
		pwm_set_gpio_level(leftMotorPin2,LOW);
		pwm_set_gpio_level(rightMotorPin1,LOW);
		pwm_set_gpio_level(rightMotorPin2,LOW);
#ifdef SERIAL_DEBUG_MODE    
		printf("All on zero\n");
#endif
		return;
	}
	
	if (speedLeft > 0) {
		pwm_set_gpio_level(leftMotorPin1, speedLeft);
		pwm_set_gpio_level(leftMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Left %d,0\n",speedLeft);
#endif
	} 
	else {
		pwm_set_gpio_level(leftMotorPin1, 0);
		pwm_set_gpio_level(leftMotorPin2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
		printf("Left 0,%d\n",-speedLeft);
#endif
	}
 
	if (speedRight > 0) {
		pwm_set_gpio_level(rightMotorPin1, speedRight);
		pwm_set_gpio_level(rightMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Right %d,0\n",speedRight);
#endif
	}
	else {
		pwm_set_gpio_level(rightMotorPin2, 0);		
		pwm_set_gpio_level(rightMotorPin2, -speedRight);
#ifdef SERIAL_DEBUG_MODE
		printf("Right 0,%d\n",-speedRight);
#endif
	}
}

static void stopLeftEngine() {
	pwm_set_gpio_level(leftMotorPin1,LOW);
	pwm_set_gpio_level(leftMotorPin2,LOW);
}

static void stopRightEngine() {
	pwm_set_gpio_level(rightMotorPin1,LOW);
	pwm_set_gpio_level(rightMotorPin2,LOW);
}

static void moveWithDistance(float moveData) {
	bool stopLeft = false;
	bool stopRight = false;
	float distance;
	if (moveData > 0) {
		distance = moveData;
		go(currentPower,currentPower);
	} else if (moveData < 0) {
		distance = - moveData;
		go(-currentPower,-currentPower);
	} else {
		go(0,0);
		return;
	}
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			leftCurrentDistance = leftCounts / leftPPI;
			if ((distance - leftCurrentDistance) <= 0.2) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			rightCurrentDistance = rightCounts / rightPPI;
			if ((distance - rightCurrentDistance) <= 0.2) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
	leftCurrentDistance = 0;
	rightCurrentDistance = 0;
}

static void rotate90Left() {
	bool stopLeft = false;
	bool stopRight = false;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate 90 left with left=%ld and right=%ld\r\n", countRotate90Left, countRotate90Right);
#endif	
	go(-currentPower,currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= countRotate90Left) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= countRotate90Right) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
}

static void rotate90Right() {
	bool stopLeft = false;
	bool stopRight = false;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate 90 right with left=%ld and right=%ld\r\n", countRotate90Left, countRotate90Right);
#endif	
	
	go(currentPower,-currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= countRotate90Left) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= countRotate90Right) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
}

static void rotateLeftDegree(int nr) {
	bool stopLeft = false;
	bool stopRight = false;
	long int leftTargetCounts = countRotate1Inner*nr;
	long int rightTargetCounts = countRotate1Outer*nr;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate %d left with left=%ld and right=%ld\r\n", nr, leftTargetCounts, rightTargetCounts);
#endif		
	go(-currentPower,currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= leftTargetCounts) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= rightTargetCounts) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
}

static void rotateRightDegree(int nr) {
	bool stopLeft = false;
	bool stopRight = false;
	long int rightTargetCounts = countRotate1Inner*nr;
	long int leftTargetCounts = countRotate1Outer*nr;
#ifdef SERIAL_DEBUG_MODE
	printf("Rotate %d right with left=%ld and right=%ld\r\n", nr, leftTargetCounts, rightTargetCounts);
#endif	
	go(currentPower,-currentPower);
	while ( !stopLeft || !stopRight){
		if (!stopLeft) {
			if (leftCounts >= leftTargetCounts) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			if (rightCounts >= rightTargetCounts) {
				stopRightEngine();
				stopRight = true;
			}
		}
	}
	go(0,0);
}

void clearEncoders() {
	leftCounts = 0;
	rightCounts = 0;
}

/*
 * Move the platform with dinstance or rotate with encoders
 */
void moveOrRotateWithDistance(float moveData, int rotateData) {
	go(0,0);
	clearEncoders();
	if (rotateData == 0) {
#ifdef SERIAL_DEBUG_MODE
	printf("Move linear to %f\r\n", moveData);
#endif
		moveWithDistance(moveData);
	} else if (fabs(moveData) <= 0.01) {
		if (rotateData == -1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees left\r\n");
#endif
			rotate90Left();
		} else if (rotateData == 1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees right\r\n");
#endif
			rotate90Right();
		} else if (rotateData < 0) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees left\r\n",-rotateData);
#endif
			rotateLeftDegree(-rotateData);
		} else {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees right\r\n",rotateData);
#endif
			rotateRightDegree(rotateData);
		}
	}
}
