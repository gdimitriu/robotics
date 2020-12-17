/*
 * Copyright 2018 Gabriel Dimitriu

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
#include <RechargeSubSystem.h>

void wakeUpNow()        // here the interrupt is handled after wakeup
{
	// execute code here after wake-up before returning to the loop() function
	// timers and code using timers (serial.print and more...) will not work here.
	// we don't really need to execute any special functions here, since we
	// just want the thing to wake up
}

RechargeSubSystem::RechargeSubSystem()
{
	bufferMessages = calloc(4, sizeof(char *));
	bufferMessages[0] = calloc(80, sizeof(char));
	bufferMessages[1] = calloc(80, sizeof(char));
	bufferMessages[2] = calloc(80, sizeof(char));
	bufferMessages[3] = calloc(80, sizeof(char));
}

void RechargeSubSystem::setDebugMode(bool debug)
{
	this->debugMode = debug;
}

void RechargeSubSystem::setEnginesSubSystem(EnginesSubSystem *engines)
{
	this->enginesSubSystem = engines;
}

void RechargeSubSystem::setSettings(AutonomousDroidSettings *settings)
{
	this->droidSettings = settings;
}

void RechargeSubSystem::setNavigationSubSystem(NavigationSubSystem *navigation)
{
	this->navigationSubSystem = navigation;
}

void RechargeSubSystem::setI2CBus(I2CSubSystem *i2c)
{
	this->i2cBus = i2c;
}

void RechargeSubSystem::addPhotoSensors(int leftPin, int rightPin)
{
	this->pinLeftLightSensor = leftPin;
	this->pinRightLightSensor = rightPin;
}

void RechargeSubSystem::setWakeUpSensor(int pin, int detectState)
{
	this->pinWakeUpSensor = pin;
	this->detectWakeUpSensor = detectState;
	this->stateForWakeUp = RISING;
	if (this->detectWakeUpSensor == LOW) {
		this->stateForWakeUp = FALLING;
	}
}

void RechargeSubSystem::initialize()
{
	pinMode(pinLeftLightSensor, INPUT);
	pinMode(pinRightLightSensor, INPUT);
	pinMode(pinWakeUpSensor, INPUT);
	attachInterrupt(digitalPinToInterrupt(pinWakeUpSensor), wakeUpNow, stateForWakeUp);
}

/*
* Move the droid to the light.
*/
int RechargeSubSystem::goToLight()
{
	long photoL = analogRead(pinLeftLightSensor);
	long photoR = analogRead(pinRightLightSensor);
	if (debugMode && i2cBus->hasLCD()) {
		String message = "L:=";
		message = message + photoL;
		message.toCharArray(bufferMessages[0], 17);
		message = "R:=";
		message = message + photoR;
		message.toCharArray(bufferMessages[1], 17);

		Serial.println(bufferMessages[0]);
		Serial.println(bufferMessages[1]);
		i2cBus->print(bufferMessages, 2);
	}
	//stay in place
	if (abs(photoL - photoR) < droidSettings->getErrorRechargeLight()) {
		if (photoL > droidSettings->getMaxRechargeLight()) {
			//stay in light
			enginesSubSystem->go(0, 0);
			sleepNow();
			has_turn = 0;
			return 0;
		}
		else if (photoL > droidSettings->getMinRechargeLight()) {
			enginesSubSystem->goSlowForward();
			has_turn = 0;
			return 0;
		}
		else {
			if (has_turn == 0) {
				//move arbirary
				enginesSubSystem->go90Left();
				enginesSubSystem->goSlowForward();
				has_turn = 1;
				return HAS_MOVE_TO_LEFT;
			}
			enginesSubSystem->goSlowForward();
		}
	}
	if (photoL - photoR < 0) {
		//move to right
		enginesSubSystem->go90Right();
		has_turn = 1;
	}
	else if (photoL - photoR > 0) {
		//move to left
		enginesSubSystem->go90Left();
		has_turn = 0;
	}
	enginesSubSystem->goSlowForward();
	return 0;
}

void RechargeSubSystem::sleepNow()         // here we put the arduino to sleep
{
	/* Now is the time to set the sleep mode. In the Atmega8 datasheet
	* http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
	* there is a list of sleep modes which explains which clocks and
	* wake up sources are available in which sleep mode.
	*
	* In the avr/sleep.h file, the call names of these sleep modes are to be found:
	*
	* The 5 different modes are:
	*     SLEEP_MODE_IDLE         -the least power savings
	*     SLEEP_MODE_ADC
	*     SLEEP_MODE_PWR_SAVE
	*     SLEEP_MODE_STANDBY
	*     SLEEP_MODE_PWR_DOWN     -the most power savings
	*
	* For now, we want as much power savings as possible, so we
	* choose the according
	* sleep mode: SLEEP_MODE_PWR_DOWN
	*
	*/
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here

	sleep_enable();          // enables the sleep bit in the mcucr register
							 // so sleep is possible. just a safety pin

							 /* Now it is time to enable an interrupt. We do it here so an
							 * accidentally pushed interrupt button doesn't interrupt
							 * our running program. if you want to be able to run
							 * interrupt code besides the sleep function, place it in
							 * setup() for example.
							 *
							 * In the function call attachInterrupt(A, B, C)
							 * A   can be either 0 or 1 for interrupts on pin 2 or 3.
							 *
							 * B   Name of a function you want to execute at interrupt for A.
							 *
							 * C   Trigger mode of the interrupt pin. can be:
							 *             LOW        a low level triggers
							 *             CHANGE     a change in level triggers
							 *             RISING     a rising edge of a level triggers
							 *             FALLING    a falling edge of a level triggers
							 *
							 * In all but the IDLE sleep modes only LOW can be used.
							 */

	attachInterrupt(digitalPinToInterrupt(pinWakeUpSensor), wakeUpNow, stateForWakeUp); // use interrupt 0 (pin 2) and run function
										// wakeUpNow when pin 2 gets LOW

	sleep_mode();            // here the device is actually put to sleep!!
							 // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

	sleep_disable();         // first thing after waking from sleep:
							 // disable sleep...
	detachInterrupt(digitalPinToInterrupt(pinWakeUpSensor));      // disables interrupt 0 on pin 2 so the
							 // wakeUpNow code will not be executed
							 // during normal running time.

}