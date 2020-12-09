/*
 * CEnginePCA9685EncMX1509.h
 * Engine for raspberry Pi Zero with PCA9865 and MX1509 with encoders
 *  Created on: Oct 2, 2020
 *      Author: Gabriel Dimitriu
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.

 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <pigpio.h>
#include <Adafruit_PWMServoDriver.h>
#include <CEngineWithEncoder.h>

#ifndef CONTROLL_ENGINES_CEnginePCA9685EncMX1509_H_
#define CONTROLL_ENGINES_CEnginePCA9685EncMX1509_H_

class CEnginePCA9685EncMX1509: public CEngineWithEncoder {
public:
	/*
	 * engineNR is the number of engine ex:0(left front),1 (left back),2 (right back),3 (right front)
	 * port is the gpio port on which the encoder is wired
	 * engPin1 and engPin2 are the port on pwm driver which are wired to the MX1509 pins
	 * driver is the PCA9685 I2C PWM expander (do not delete this because it will be used also for servos)
	 */
	CEnginePCA9685EncMX1509(unsigned int engineNr, unsigned int port, int edge,
			unsigned int engPin1, unsigned int engPin2);
	virtual ~CEnginePCA9685EncMX1509();
	virtual void dumpInfo();
	/*
	 * set the pwm driver which generate PWM for engines and servo
	 */
	void setPWMDriver(Adafruit_PWMServoDriver *driver);
	/*
	 * get the pwm driver which generate PWM for engines and servo
	 */
	void *getPWMDriver();
	/*
	 * This static method will be called by the piggio library for ISR and using the holder will be
	 * forward to the correct engine instance.
	 * the holder will be be the pointer to the running engine.
	 * gpio is the port on which the encoder is wired
	 * level is the rising/fall edge on which the isr will be trigger
	 */
	static void ISRMain(int gpio, int level, uint32_t tick, void *holder);
	/* isr for the ecoder is specific to each engine of the droid*/
	void encISR(int gpio, int level, uint32_t tick);
	virtual unsigned int getEngineNr();
	/* coast the engine, engine will have no brake */
	virtual void coastEngine();
	/* break the engine */
	virtual void breakEngine();
	/* set PPI for the encoder */
	virtual void setPPI(float ppi);
	/* get the encoder value */
	virtual unsigned long getEncoder();
	/* get the current power of the engine */
	virtual unsigned int getEnginePower();
	/* set the engine current power */
	virtual void setEnginePower(unsigned int power);
	/* move of specific distance in cm + is forward - is backward */
	virtual void moveDistance(float distance);
	/*
	 * This will be use to rotate the droid
	 * use for left direction = -1 or right = 1
	 * encoderNr is the number of ticks to move
	 */
	virtual void moveEncoderNr(unsigned long encoderNr, int direction);
	/*
	 * this will move the distance base of encoder value
	 * this will be run inside a thread.
	 * Each engine will compute and go at the specified distance.
	 */
	friend void* CEnginePCA9685EncMX1509_moveDistance(void *engine);
	/*
	 * this will rotate the well with the number of encoder ticks into a thread
	 */
	friend void* CEnginePCA9685EncMX1509_moveEncoderNr(void *engine);
	/*
	 * get the actual distance that the droid had move
	*/
	virtual float getActualDistance();
	/*
	 * set the maximum engine power
	*/
	virtual void setMaxEnginePower(unsigned int newPower);
	/*
	 * set the minimum engine power
	*/
	virtual void setMinEnginePower(unsigned int newPower);
	/*
	 * get the maximum engine power
	*/
	virtual unsigned int getMaxEnginePower();
	/*
	 * get the minimum engine power
	*/
	virtual unsigned int getMinEnginePower();
	/*
	 * get the low power distance
	*/
	virtual unsigned long getLowPowerDistance();
	/*
	 * set the low power distance
	*/
	virtual void setLowPowerDistance(unsigned long distance);
protected:
	/* start encoder thread */
	virtual void startEncoder();
	/* stop encoder thread */
	virtual void stopEncoder();
	/* clear/reset the encoder value */
	virtual void clearEncoder();
	virtual void startMoving();
private:
	/* maximum power of the engine between 0 and 4095 */
	static unsigned int maxEnginePower;
	/* minimum power of the engine between 0 and 4095 */
	static unsigned int minEnginePower;
	/* the distance in front of droid that will be move using low power*/
	static unsigned long lowPowerDistance;
	unsigned int m_engineNr;
	unsigned int m_encPort;
	int m_encEdge;
	unsigned int m_enginePin1;
	unsigned int m_enginePin2;
	unsigned int m_enginePower;
	float m_ppi;
	volatile unsigned long m_encoderCount;
	float m_targetDistance;
	float m_currentDistance;
	volatile unsigned long m_targetEncoder;
	Adafruit_PWMServoDriver *m_pwmDriver;
	pthread_t m_goTh;
	pthread_attr_t m_goThAttr;
	struct sched_param m_goSchedParam;
	unsigned int m_actualPower;
	float m_requestedDistance;
	pthread_mutex_t m_isrMutex;
	pthread_cond_t m_isrCond;
};

#endif /* CONTROLL_ENGINES_CEnginePCA9685EncMX1509_H_ */
