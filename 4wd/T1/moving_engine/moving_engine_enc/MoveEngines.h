#ifndef __MOVEENGINES_H__ 
#define __MOVEENGINES_H__ 

void moveLinear(float distance);
void rotateDegree(long nr);
void go(int speedLeft, int speedRight);
void breakAllEngines();

void resetCounters();
uint16_t getLeftFrontEncoderCount();
uint16_t getRightFrontEncoderCount();
uint16_t getLeftBackEncoderCount();
uint16_t getRightBackEncoderCount();

void enableEncoders();
void disableEncoders();
void engineSetup();
#endif
