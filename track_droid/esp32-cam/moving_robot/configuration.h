#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__
#include <WiFi.h>

#define SERIAL_BUFFER 50

#define ABSOLUTE_MAX_POWER 65550

#define SERVER_PORT 8080

/**********************************************
 * 
 * right motor pin 2
 * this is the pin from
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN2 12

/**********************************************
 * 
 * right motor pin 1
 * this is the pin from 
 * 
 **********************************************/
#define RIGHT_MOTOR_PIN1 13

/**********************************************
 * 
 * left motor pin 2
 * this is the pin from 
 * 
 **********************************************/
#define LEFT_MOTOR_PIN2 14

/**********************************************
 * 
 * left motor pin 1
 * this is the pin from 
 * 
 **********************************************/
#define LEFT_MOTOR_PIN1 15

extern int currentPower;

extern int minPower;

extern int maxPower;

extern int indexReceive;

extern bool isValidInput;

void makeCleanup();

void sendOK();

#endif
