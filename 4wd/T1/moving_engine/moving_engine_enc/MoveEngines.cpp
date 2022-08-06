#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EnableInterrupt.h>
#include "configuration.h"
#include "MoveEngines.h"

extern Adafruit_PWMServoDriver pwmDriver; 

//variables for encoders
float wheelRadius = 3.5f;
int encoderWheelSteps = 20;

static volatile uint16_t  left_front_encoder_count = 0;
static volatile uint16_t  right_front_encoder_count = 0;
static volatile uint16_t  left_back_encoder_count = 0;
static volatile uint16_t  right_back_encoder_count = 0;

static float PPI_front_left;
static float PPI_front_right;
static float PPI_back_left;
static float PPI_back_right;
bool encoderEnabled = false;
/*
 * isr for encoder pins
 */
void isrLeftFrontEncoder() {
  left_front_encoder_count++;
}

void isrRightFrontEncoder() {
  right_front_encoder_count++;
}
void isrLeftBackEncoder() {
  left_back_encoder_count++;
}

void isrRightBackEncoder() {
  right_back_encoder_count++;
}

void resetCounters() {
  left_front_encoder_count = 0;
  right_front_encoder_count = 0;
  left_back_encoder_count = 0;
  right_back_encoder_count = 0;
}

void disableEncoders() {
  if (!encoderEnabled)
    return;
  disableInterrupt(leftFrontEncoderPin);
  disableInterrupt(rightFrontEncoderPin);
  disableInterrupt(leftBackEncoderPin);
  disableInterrupt(rightBackEncoderPin);  
  encoderEnabled = false;
}

void enableEncoders() {
  if (encoderEnabled)
    return;
  enableInterrupt(leftFrontEncoderPin, isrLeftFrontEncoder, RISING);
  enableInterrupt(rightFrontEncoderPin, isrRightFrontEncoder, RISING);
  enableInterrupt(leftBackEncoderPin, isrLeftBackEncoder, RISING);
  enableInterrupt(rightBackEncoderPin, isrRightBackEncoder, RISING);
  encoderEnabled = true;
  resetCounters();
}

void engineSetup() {
  PPI_front_left = encoderWheelSteps/(2*PI*wheelRadius);
  PPI_front_right = encoderWheelSteps/(2*PI*wheelRadius);
  PPI_back_left = encoderWheelSteps/(2*PI*wheelRadius);
  PPI_back_right = encoderWheelSteps/(2*PI*wheelRadius);
  //enable encoders ISR
  pinMode(leftFrontEncoderPin, INPUT_PULLUP);
  pinMode(rightFrontEncoderPin, INPUT_PULLUP);
  pinMode(leftBackEncoderPin, INPUT_PULLUP);
  pinMode(rightBackEncoderPin, INPUT_PULLUP);
  encoderEnabled = false;
}

uint16_t getLeftFrontEncoderCount() {
  return left_front_encoder_count;
}

uint16_t getRightFrontEncoderCount() {
  return right_front_encoder_count;
}

uint16_t getLeftBackEncoderCount() {
  return left_back_encoder_count;
}

uint16_t getRightBackEncoderCount() {
  return right_back_encoder_count;
}

void stopLeftEngines() {
    pwmDriver.setPWM(leftFrontMotorPin1, 0, 4095);
    pwmDriver.setPWM(leftFrontMotorPin2, 0, 4095);
    pwmDriver.setPWM(leftBackMotorPin1, 0, 4095);
    pwmDriver.setPWM(leftBackMotorPin2, 0, 4095);
}

void stopRightEngines() {
    pwmDriver.setPWM(rightFrontMotorPin1, 0, 4095);
    pwmDriver.setPWM(rightFrontMotorPin2, 0, 4095);
    pwmDriver.setPWM(rightBackMotorPin1, 0, 4095);
    pwmDriver.setPWM(rightBackMotorPin2, 0, 4095);
}
void breakAllEngines() {
    pwmDriver.setPWM(leftFrontMotorPin1, 0, 4095);
    pwmDriver.setPWM(leftFrontMotorPin2, 0, 4095);
    pwmDriver.setPWM(leftBackMotorPin1, 0, 4095);
    pwmDriver.setPWM(leftBackMotorPin2, 0, 4095);
    pwmDriver.setPWM(rightFrontMotorPin1, 0, 4095);
    pwmDriver.setPWM(rightFrontMotorPin2, 0, maxPower);    
    pwmDriver.setPWM(rightBackMotorPin1, 0, maxPower);
    pwmDriver.setPWM(rightBackMotorPin2, 0, maxPower);
}
/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {

  if (speedLeft == 0 && speedRight == 0 ) {
    pwmDriver.setPWM(leftFrontMotorPin1, 0, 0);
    pwmDriver.setPWM(leftFrontMotorPin2, 0, 0);
    pwmDriver.setPWM(leftBackMotorPin1, 0, 0);
    pwmDriver.setPWM(leftBackMotorPin2, 0, 0);
    pwmDriver.setPWM(rightFrontMotorPin1, 0, 0);
    pwmDriver.setPWM(rightFrontMotorPin2, 0, 0);
    pwmDriver.setPWM(rightBackMotorPin1, 0, 0);
    pwmDriver.setPWM(rightBackMotorPin2, 0, 0);
    return;
  }
  if (speedLeft > 0) {
    pwmDriver.setPWM(leftFrontMotorPin1, 0, speedLeft);
    pwmDriver.setPWM(leftFrontMotorPin2, 0, 0);
    pwmDriver.setPWM(leftBackMotorPin1, 0, speedLeft);
    pwmDriver.setPWM(leftBackMotorPin2, 0, 0);
  } 
  else {
    pwmDriver.setPWM(leftFrontMotorPin1, 0, 0);
    pwmDriver.setPWM(leftFrontMotorPin2, 0, -speedLeft);    
    pwmDriver.setPWM(leftBackMotorPin1, 0, 0);
    pwmDriver.setPWM(leftBackMotorPin2, 0, -speedLeft);    
  }
 
  if (speedRight > 0) {
    pwmDriver.setPWM(rightFrontMotorPin1, 0, speedRight);
    pwmDriver.setPWM(rightFrontMotorPin2, 0, 0);    
    pwmDriver.setPWM(rightBackMotorPin1, 0, speedRight);
    pwmDriver.setPWM(rightBackMotorPin2, 0, 0);
  }else {
    pwmDriver.setPWM(rightFrontMotorPin1, 0, 0);    
    pwmDriver.setPWM(rightFrontMotorPin2, 0, -speedRight);    
    pwmDriver.setPWM(rightBackMotorPin1, 0, 0);
    pwmDriver.setPWM(rightBackMotorPin2, 0, -speedRight);
  }
}


void moveLinear(float distance) {
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  if (distance > 0) {
    go(currentPower,currentPower);
  } else if (distance < 0) {
    go(-currentPower,-currentPower);
    distance = -distance;
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2) {
        currentRightFrontPosition = right_front_encoder_count/PPI_front_right;
      } else {
        stopRightEngines();
        stopRight=true;
      }
    }
  }
  breakAllEngines();
  delay(100);
  go(0,0);
}

void rotateDegree(long nr) {
  if (nr < 0) {
    go(-currentPower,currentPower);
    nr = -nr;
    while(left_front_encoder_count < countRotate1Inner*nr && right_front_encoder_count < countRotate1Outer*nr);
  } else if (nr > 0) {
    go(currentPower,-currentPower);
    while(left_front_encoder_count < countRotate1Outer*nr && right_front_encoder_count < countRotate1Inner*nr);
  } else {
    return;
  }
  breakAllEngines();
  delay(100);  
  go(0,0);
}
