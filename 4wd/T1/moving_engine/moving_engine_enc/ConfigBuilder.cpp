#include "ConfigBuilder.h"
#include "configuration.h"

unsigned int leftFrontEncoderPin = 5;
unsigned int rightFrontEncoderPin = 3;
unsigned int leftBackEncoderPin = 4;
unsigned int rightBackEncoderPin = 2;
unsigned int ssPin = 10;

unsigned int leftFrontMotorPin1 = 2;
unsigned int leftFrontMotorPin2 = 3;
unsigned int rightFrontMotorPin1 = 0;
unsigned int rightFrontMotorPin2 = 1;
unsigned int leftBackMotorPin1 = 13;
unsigned int leftBackMotorPin2 = 12;
unsigned int rightBackMotorPin1 = 15;
unsigned int rightBackMotorPin2 = 14;


int countRotate90Left = 1;
int countRotate90Right = 1;
int countRotate1Inner = 1;
int countRotate1Outer = 1;
bool hasRFID = false;
