/*!
 *  @file Adafruit_PWMServoDriver.cpp
 *
 *  @mainpage Adafruit 16-channel PWM & Servo driver
 *
 *  @section intro_sec Introduction
 *
 *  This is a library for the 16-channel PWM & Servo driver.
 *
 *  Designed specifically to work with the Adafruit PWM & Servo driver.
 *
 *  Pick one up today in the adafruit shop!
 *  ------> https://www.adafruit.com/product/815
 *
 *  These displays use I2C to communicate, 2 pins are required to interface.
 *
 *  Adafruit invests time and resources providing this open source code,
 *  please support Adafruit andopen-source hardware by purchasing products
 *  from Adafruit!
 *
 *  @section author Author
 *
 *  Limor Fried/Ladyada (Adafruit Industries).
 *
 *  @section license License
 *
 *  BSD license, all text above must be included in any redistribution
 */
/*
 * Porting to RaspberryPi.
 * Copyright (C) 2020 Gabriel Dimitriu
 * All rights reserved.
 */

#include "Adafruit_PWMServoDriver.h"
#include <algorithm>
#include <stdio.h>

//#define ENABLE_DEBUG_OUTPUT

/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 */
Adafruit_PWMServoDriver::Adafruit_PWMServoDriver()
    : m_i2caddr(PCA9685_I2C_ADDRESS), m_oscillator_freq(FREQUENCY_OSCILLATOR) {
	m_i2cHandler = i2cOpen(I2C_BUS, m_i2caddr, 0);
}

/*!
 *  @brief  Instantiates a new PCA9685 PWM driver chip with the I2C address on a
 * TwoWire interface
 *  @param  t_addr The 7-bit I2C address to locate this chip, default is 0x40
 */
Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(const uint8_t t_addr)
    : m_i2caddr(t_addr), m_oscillator_freq(FREQUENCY_OSCILLATOR){
	m_i2cHandler = i2cOpen(I2C_BUS, m_i2caddr, 0);
}

Adafruit_PWMServoDriver::Adafruit_PWMServoDriver(const uint8_t t_bus, const uint8_t t_addr)
: m_i2caddr(t_addr), m_oscillator_freq(FREQUENCY_OSCILLATOR){
	m_i2cHandler = i2cOpen(t_bus, m_i2caddr, 0);
}
Adafruit_PWMServoDriver::~Adafruit_PWMServoDriver() {
	i2cClose(m_i2cHandler);
	pthread_mutex_destroy(&m_mutexSetPwm);
}
/*!
 *  @brief  Setups the I2C interface and hardware
 *  @param  t_prescale
 *          Sets External Clock (Optional)
 */
void Adafruit_PWMServoDriver::begin(uint8_t t_prescale) {
  reset();
  if (t_prescale) {
    setExtClk(t_prescale);
  } else {
    // set a default frequency
    setPWMFreq(1000);
  }
  // set the default internal frequency
  setOscillatorFrequency(FREQUENCY_OSCILLATOR);
  pthread_mutex_init(&m_mutexSetPwm, NULL);
}

/*!
 *  @brief  Sends a reset command to the PCA9685 chip over I2C
 */
void Adafruit_PWMServoDriver::reset() {
  write8(PCA9685_MODE1, MODE1_RESTART);
  delay(10);
}

/*!
 *  @brief  Puts board into sleep mode
 */
void Adafruit_PWMServoDriver::sleep() {
  uint8_t awake = read8(PCA9685_MODE1);
  uint8_t sleep = awake | MODE1_SLEEP; // set sleep bit high
  write8(PCA9685_MODE1, sleep);
  delay(5); // wait until cycle ends for sleep to be active
}

/*!
 *  @brief  Wakes board from sleep
 */
void Adafruit_PWMServoDriver::wakeup() {
  uint8_t sleep = read8(PCA9685_MODE1);
  uint8_t wakeup = sleep & ~MODE1_SLEEP; // set sleep bit low
  write8(PCA9685_MODE1, wakeup);
}

/*!
 *  @brief  Sets EXTCLK pin to use the external clock
 *  @param  t_prescale
 *          Configures the prescale value to be used by the external clock
 */
void Adafruit_PWMServoDriver::setExtClk(uint8_t t_prescale) {
  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode); // go to sleep, turn off internal oscillator

  // This sets both the SLEEP and EXTCLK bits of the MODE1 register to switch to
  // use the external clock.
  write8(PCA9685_MODE1, (newmode |= MODE1_EXTCLK));

  write8(PCA9685_PRESCALE, t_prescale); // set the prescaler

  delay(5);
  // clear the SLEEP bit to start
  write8(PCA9685_MODE1, (newmode & ~MODE1_SLEEP) | MODE1_RESTART | MODE1_AI);

#ifdef ENABLE_DEBUG_OUTPUT
  printf("Mode now 0x\n", read8(PCA9685_MODE1));
  fflush(stdout);
#endif
}

/*!
 *  @brief  Sets the PWM frequency for the entire chip, up to ~1.6 KHz
 *  @param  t_freq Floating point frequency that we will attempt to match
 */
void Adafruit_PWMServoDriver::setPWMFreq(float t_freq) {
#ifdef ENABLE_DEBUG_OUTPUT
  printf("Attempting to set freq %f\n", t_freq);
  fflush(stdout);
#endif
  // Range output modulation frequency is dependant on oscillator
  if (t_freq < 1)
    t_freq = 1;
  if (t_freq > 3500)
    t_freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

  float prescaleval = ((m_oscillator_freq / (t_freq * 4096.0)) + 0.5) - 1;
  if (prescaleval < PCA9685_PRESCALE_MIN)
    prescaleval = PCA9685_PRESCALE_MIN;
  if (prescaleval > PCA9685_PRESCALE_MAX)
    prescaleval = PCA9685_PRESCALE_MAX;
  uint8_t prescale = (uint8_t)prescaleval;

#ifdef ENABLE_DEBUG_OUTPUT
  printf("Final pre-scale: %d\n", prescale);
  fflush(stdout);
#endif

  uint8_t oldmode = read8(PCA9685_MODE1);
  uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
  write8(PCA9685_MODE1, newmode);                             // go to sleep
  write8(PCA9685_PRESCALE, prescale); // set the prescaler
  write8(PCA9685_MODE1, oldmode);
  delay(5);
  // This sets the MODE1 register to turn on auto increment.
//  write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);

#ifdef ENABLE_DEBUG_OUTPUT
  printf("Mode now 0x%d\n", read8(PCA9685_MODE1));
  fflush(stdout);
#endif
}

/*!
 *  @brief  Sets the output mode of the PCA9685 to either
 *  open drain or push pull / totempole.
 *  Warning: LEDs with integrated zener diodes should
 *  only be driven in open drain mode.
 *  @param  totempole Totempole if true, open drain if false.
 */
void Adafruit_PWMServoDriver::setOutputMode(bool t_totempole) {
  uint8_t oldmode = read8(PCA9685_MODE2);
  uint8_t newmode;
  if (t_totempole) {
    newmode = oldmode | MODE2_OUTDRV;
  } else {
    newmode = oldmode & ~MODE2_OUTDRV;
  }
  write8(PCA9685_MODE2, newmode);
#ifdef ENABLE_DEBUG_OUTPUT
  printf("Setting output mode: %d  by setting MODE2 to %d\n", t_totempole ? "totempole" : "open drain",newmode);
  fflush(stdout);
#endif
}

/*!
 *  @brief  Reads set Prescale from PCA9685
 *  @return prescale value
 */
uint8_t Adafruit_PWMServoDriver::readPrescale(void) {
  return read8(PCA9685_PRESCALE);
}

/*!
 *  @brief  Gets the PWM output of one of the PCA9685 pins
 *  @param  num One of the PWM output pins, from 0 to 15
 *  @return requested PWM output value
 */
/* something is wrong 4 bytes read and one returned */
/*
uint8_t Adafruit_PWMServoDriver::getPWM(uint8_t num) {
  _i2c->requestFrom((int)_i2caddr, PCA9685_LED0_ON_L + 4 * num, (int)4);
  return _i2c->read();
}
*/
/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins
 *  @param  t_num One of the PWM output pins, from 0 to 15
 *  @param  t_on At what point in the 4096-part cycle to turn the PWM output ON
 *  @param  t_off At what point in the 4096-part cycle to turn the PWM output OFF
 */
void Adafruit_PWMServoDriver::setPWM(uint8_t t_num, uint16_t t_on, uint16_t t_off) {
#ifdef ENABLE_DEBUG_OUTPUT
  printf("Setting PWM %d : %d -> %d \n", t_num, t_on, t_off);
  fflush(stdout);
#endif
  pthread_mutex_lock(&m_mutexSetPwm);
  uint8_t buffer[1];
  buffer[0] = t_on;
  i2cWriteByteData(m_i2cHandler, PCA9685_LED0_ON_L + 4 * t_num, buffer[0]);
  buffer[0] = 0;
  buffer[0] = t_on >> 8;
  i2cWriteByteData(m_i2cHandler, PCA9685_LED0_ON_L + 4 * t_num + 1, buffer[0]);
  buffer[0] = 0;
  buffer[0] = t_off;
  i2cWriteByteData(m_i2cHandler, PCA9685_LED0_ON_L + 4 * t_num + 2, buffer[0]);
  buffer[0] = 0;
  buffer[0] = t_off >> 8;
  i2cWriteByteData(m_i2cHandler, PCA9685_LED0_ON_L + 4 * t_num + 3, buffer[0]);
  pthread_mutex_unlock(&m_mutexSetPwm);
}

/*!
 *   @brief  Helper to set pin PWM output. Sets pin without having to deal with
 * on/off tick placement and properly handles a zero value as completely off and
 * 4095 as completely on.  Optional invert parameter supports inverting the
 * pulse for sinking to ground.
 *   @param  t_num One of the PWM output pins, from 0 to 15
 *   @param  t_val The number of ticks out of 4096 to be active, should be a value
 * from 0 to 4095 inclusive.
 *   @param  t_invert If true, inverts the output, defaults to 'false'
 */
void Adafruit_PWMServoDriver::setPin(uint8_t t_num, uint16_t t_val, bool t_invert) {
  // Clamp value between 0 and 4095 inclusive.
  t_val = std::min(t_val, (uint16_t)4095);
  if (t_invert) {
    if (t_val == 0) {
      // Special value for signal fully on.
      setPWM(t_num, 4096, 0);
    } else if (t_val == 4095) {
      // Special value for signal fully off.
      setPWM(t_num, 0, 4096);
    } else {
      setPWM(t_num, 0, 4095 - t_val);
    }
  } else {
    if (t_val == 4095) {
      // Special value for signal fully on.
      setPWM(t_num, 4096, 0);
    } else if (t_val == 0) {
      // Special value for signal fully off.
      setPWM(t_num, 0, 4096);
    } else {
      setPWM(t_num, 0, t_val);
    }
  }
}

/*!
 *  @brief  Sets the PWM output of one of the PCA9685 pins based on the input
 * microseconds, output is not precise
 *  @param  t_num One of the PWM output pins, from 0 to 15
 *  @param  t_Microseconds The number of Microseconds to turn the PWM output ON
 */
void Adafruit_PWMServoDriver::writeMicroseconds(uint8_t t_num,
                                                uint16_t t_Microseconds) {
#ifdef ENABLE_DEBUG_OUTPUT
  printf("Setting PWM Via Microseconds on output %d ; %d ->\n", t_num, t_Microseconds);
  fflush(stdout);
#endif

  double pulse = t_Microseconds;
  double pulselength;
  pulselength = 1000000; // 1,000,000 us per second

  // Read prescale
  uint16_t prescale = readPrescale();

#ifdef ENABLE_DEBUG_OUTPUT
  printf("%d PCA9685 chip prescale\n",prescale);
  fflush(stdout);
#endif

  // Calculate the pulse for PWM based on Equation 1 from the datasheet section
  // 7.3.5
  prescale += 1;
  pulselength *= prescale;
  pulselength /= m_oscillator_freq;

#ifdef ENABLE_DEBUG_OUTPUT
  printf("%d us per bit\n", pulselength);
  fflush(stdout);
#endif

  pulse /= pulselength;

#ifdef ENABLE_DEBUG_OUTPUT
  printf("%d pulse for PWM\n", pulse);
  fflush(stdout);
#endif

  setPWM(t_num, 0, pulse);
}

/*!
 *  @brief  Getter for the internally tracked oscillator used for freq
 * calculations
 *  @returns The frequency the PCA9685 thinks it is running at (it cannot
 * introspect)
 */
uint32_t Adafruit_PWMServoDriver::getOscillatorFrequency(void) {
  return m_oscillator_freq;
}

/*!
 *  @brief Setter for the internally tracked oscillator used for freq
 * calculations
 *  @param t_freq The frequency the PCA9685 should use for frequency calculations
 */
void Adafruit_PWMServoDriver::setOscillatorFrequency(uint32_t t_freq) {
  m_oscillator_freq = t_freq;
}

/******************* Low level I2C interface */
uint8_t Adafruit_PWMServoDriver::read8(uint8_t t_addr) {
	return (uint8_t) i2cReadByteData(m_i2cHandler, t_addr);
}

void Adafruit_PWMServoDriver::write8(uint8_t t_addr, uint8_t d) {
	i2cWriteByteData(m_i2cHandler, t_addr, d);
}
