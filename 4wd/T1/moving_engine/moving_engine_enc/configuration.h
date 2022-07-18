#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#define SERIAL_BUFFER 50

extern unsigned int leftFrontEncoderPin;
extern unsigned int rightFrontEncoderPin;
extern unsigned int leftBackEncoderPin;
extern unsigned int rightBackEncoderPin;
extern unsigned int ssPin;

extern unsigned int leftFrontMotorPin1;
extern unsigned int leftFrontMotorPin2;
extern unsigned int rightFrontMotorPin1;
extern unsigned int rightFrontMotorPin2;
extern unsigned int leftBackMotorPin1;
extern unsigned int leftBackMotorPin2;
extern unsigned int rightBackMotorPin1;
extern unsigned int rightBackMotorPin2;

extern int countRotate90Left;
extern int countRotate90Right;
extern int countRotate1Inner;
extern int countRotate1Outer;

extern int maxPower;
extern int currentPower;
extern int minPower;

extern float wheelRadius;
extern int encoderWheelSteps;

extern bool hasRFID;

#endif
