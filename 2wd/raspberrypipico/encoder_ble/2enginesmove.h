/*
 * 2enginesmove.h
 *
 *  Created on: June 29, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#ifndef _2ENGINESMOVE_H_
#define _2ENGINESMOVE_H_

	/**
	 * break the engines
	 */
	void breakEngines();
	
	/**
	 * move with different speed on each engine
	 * @param speedLeft left engine power
	 * @param sppedRight right engine power
	 */
	void go(int speedLeft, int speedRight);
	
	/**
	 * move to distance or rotate
	 * @param moveData distance in cm to move negative for backward
	 * @param rotateData rotate -1 to 90 left, 1 to right, negative values are left, positive values are right  
	 */
	void moveOrRotateWithDistance(float moveData, int rotateData);
	
	/**
	 * clean the encoders
	 */
	void clearEncoders();
	
	/**
	 * callback
	 * @param gpio the gpio pin which trigger the callback
	 * @param events the event triggered 
	 */
	void gpio_callback(uint gpio, uint32_t events);	
	
	/**
	 * return the left engine counts
	 */ 
	unsigned long getLeftEngineCounts();
	
	/**
	 * return right engine counts
	 */
	unsigned long getRightEngineCounts();
	
	/**
	 * set the human direction
	 * value > 0 forward
	 * value < 0 backward
	 */	
	void setHumanDirection(int value);
	/**
	 * set that a human command this using droidControl center or something else
	 * isHuman true if is human on controll (M command)
	 */
	void setHumanCommand(bool isHuman);
#endif //_2ENGINES_MOVE_H_
