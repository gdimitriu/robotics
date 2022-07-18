#ifndef __MOVEENGINES_H__ 
#define __MOVEENGINES_H__ 

void isrLeftFrontEncoder();
void isrRightFrontEncoder();
void isrLeftBackEncoder();
void isrRightBackEncoder();

void moveLinear(float distance);
void rotateDegree(long nr);
void go(int speedLeft, int speedRight);
void breakAllEngines();

void resetCounters();
long getLeftFrontEncoderCount();
long getRightFrontEncoderCount();
long getLeftBackEncoderCount();
long getRightBackEncoderCount();

void engineSetup();
#endif
