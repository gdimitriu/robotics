/*
 * 4 whell autonomous droid using bluetooth.
 * This has also encoders and distance check and moving.
 * This has also pixy2 camera with barcodes.
 * Copyright 2020 Gabriel Dimitriu
 *
 * This file is part of Robotics project

 * Robotics is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <VL53L0X.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>
#include <Arduino.h>
#include <Pixy2I2C.h>

#define RxD 2
#define TxD 3

#define BLE_DEBUG_MODE true
//#define BLE_DEBUG_SETTINGS_MODE true

#define LEFT_FRONT_ENCODER_PIN 4
#define RIGHT_FRONT_ENCODER_PIN 7
#define LEFT_BACK_ENCODER_PIN 8
#define RIGHT_BACK_ENCODER_PIN 9

//pins on pca8695
#define LEFT_FRONT_MOTOR_PIN1 2
#define LEFT_FRONT_MOTOR_PIN2 3
#define RIGHT_FRONT_MOTOR_PIN1 0
#define RIGHT_FRONT_MOTOR_PIN2 1

#define LEFT_BACK_MOTOR_PIN1 13
#define LEFT_BACK_MOTOR_PIN2 12
#define RIGHT_BACK_MOTOR_PIN1 15
#define RIGHT_BACK_MOTOR_PIN2 14

#define FRONT_SERVO_PIN 11
#define SERVO_LEFT_45 410
#define SERVO_RIGHT_45 200
#define SERVO_CENTER 310

float PPI_front_left = 20/(2*PI*3.5f);
float PPI_front_right = 20/(2*PI*3.5f);
float PPI_back_left = 20/(2*PI*3.5f);
float PPI_back_right = 20/(2*PI*3.5f);

int countRotate90Left = 1;
int countRotate90Right = 1;
int countRotate1Inner = 1;
int countRotate1Outer = 1;

//variables for encoders
volatile long left_front_encoder_count = 0;
volatile long right_front_encoder_count = 0;
volatile long left_back_encoder_count = 0;
volatile long right_back_encoder_count = 0;

int btCommand;

int maxPower = 4000;
int slowPower = 2000;
int left_right_45_encoders = 7;

// for input data from ble
boolean cleanupBT;
bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);
VL53L0X frontSensor;
Pixy2I2C pixy;
boolean enablePixy2 = false;
uint16_t SAFE_DISTANCE = 100; //minimum distance in mm
uint16_t SLOW_DISTANCE  = 300; // the maximum distance when we go with slow power
boolean enablePixy2Lamp = false;
uint16_t hPixyServo = 500; //horizontal pixy2 servo position
uint16_t vPixyServo = 500; //vertical pixy2 servo position

#define LEFT_BARCODE 0
#define RIGHT_BARCODE 5

NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}
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

uint16_t readDistance() {
  uint16_t value = frontSensor.readRangeContinuousMillimeters();//frontSensor.readRangeSingleMillimeters();
  if (frontSensor.timeoutOccurred()) {
    return 4000;
  }
  return value;
}

void moveBacwardLinear(float distance) {
  resetCounters();
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  int power = -maxPower;
  if (distance < 0) {
    distance = -distance;
    if (distance < 30) {
      power = power/2;
      if (power < -slowPower) {
        power = -slowPower;
      }
    }
    go(power, power);
    
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (BTSerial.available() > 1) {
      break;
    }
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2 ){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2 ) {
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
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftFrontPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightFrontPosition);
#endif  
}

/*
 * return -1 if nothing is detected or barecode if is detected
 */
int8_t detectBarcode() {
  int8_t res;
  // Get latest data from Pixy, including main vector, new intersections and new barcodes.
  res = pixy.line.getMainFeatures();

  // If error or nothing detected just ignore
  if (res<=0) 
  {
    return -1;
  }
  if (res&LINE_BARCODE)
  {
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Barcode detected = ");
    BTSerial.println(pixy.line.barcodes->m_code);
#endif
    return pixy.line.barcodes->m_code;
  }
}
void moveForwardLinear(float distance) {
  if (enablePixy2 && enablePixy2Lamp) {
    pixy.setLamp(1, 1);
  }
  resetCounters();
  float currentLeftFrontPosition = 0.0f;
  float currentRightFrontPosition = 0.0f;
  float currentLeftBackPosition = 0.0f;
  float currentRightBackPosition = 0.0f;
  int power = maxPower;
  boolean isGoingSlower = false;
  pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
  delay(SERVO_CENTER);
  if (distance > 0) {
    if ( readDistance() < SLOW_DISTANCE || distance < 30) {
      power = power/2;
      if (power > slowPower) {
        power = slowPower;
      }
      isGoingSlower = true;      
    }
    go(power, power);
  } else {
    return;
  }
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (BTSerial.available() > 1) {
      break;
    }
    uint16_t obstacle = readDistance();
    if ( !isGoingSlower  && obstacle < SLOW_DISTANCE) {
      power = power/2;
      if (power > slowPower) {
        power = slowPower;
      }      
      go(power,power);
      isGoingSlower = true;
    }
    if (obstacle <= SAFE_DISTANCE) {
      breakAllEngines();
      delay(100);
      stopLeft=true;
      stopRight=true;
    }
    if (enablePixy2) {
      int8_t barcode = detectBarcode();
      if (barcode >= 0) {
        breakAllEngines();
        delay(100);
        go(0,0);
        //save state
        long left_front_encoder_count_s = left_front_encoder_count;
        long right_front_encoder_count_s = right_front_encoder_count;
        long left_back_encoder_count_s = left_back_encoder_count;
        long right_back_encoder_count_s = right_back_encoder_count;
        float currentLeftFrontPosition_s = currentLeftFrontPosition;
        float currentRightFrontPosition_s = currentRightFrontPosition;
        float currentLeftBackPosition_s = currentLeftBackPosition;
        float currentRightBackPosition_s = currentRightBackPosition;
        //move accordingly to the barcode
        if (enablePixy2Lamp)
          pixy.setLamp(0, 0);
        switch(barcode) {
          case LEFT_BARCODE:
            rotate_safe(-left_right_45_encoders);
            rotate_safe(-left_right_45_encoders);
            break;
          case RIGHT_BARCODE:
            rotate_safe(left_right_45_encoders);
            rotate_safe(left_right_45_encoders);
            break;
        }
        if (enablePixy2Lamp)
          pixy.setLamp(1, 1);

        //restore state to continue the desired distance
        left_front_encoder_count = left_front_encoder_count_s;
        right_front_encoder_count = right_front_encoder_count_s;
        left_back_encoder_count = left_back_encoder_count_s;
        right_back_encoder_count = right_back_encoder_count_s;
        currentLeftFrontPosition = currentLeftFrontPosition_s;
        currentRightFrontPosition = currentRightFrontPosition_s;
        currentLeftBackPosition = currentLeftBackPosition_s;
        currentRightBackPosition = currentRightBackPosition_s;
        stopLeft=false;
        stopRight=false;
        go(power,power);
      }
    }
    if (!stopLeft) {
      if((distance - currentLeftFrontPosition) > 0.2 ){
        currentLeftFrontPosition = left_front_encoder_count/PPI_front_left;
      } else {
        stopLeftEngines();
        stopLeft = true;
      }
    } else {
      stopLeftEngines();
    }
    if (!stopRight) {
      if ((distance-currentRightFrontPosition) > 0.2) {
        currentRightFrontPosition = right_front_encoder_count/PPI_front_right;
      } else {
        stopRightEngines();
        stopRight=true;
      }
    } else {
      stopRightEngines();
    }
  }
  breakAllEngines();
  delay(100);
  go(0,0);
  if (enablePixy2 && enablePixy2Lamp) {
    pixy.setLamp(0, 0);
  }
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftFrontPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightFrontPosition);
#endif  
}

boolean stopLeftFront = false;
boolean stopRightFront = false;
boolean stopLeftBack = false;
boolean stopRightBack = false;

void rotateLeft(long nr) {
  resetCounters();
  stopLeftFront = false;
  stopRightFront = false;
  stopLeftBack = false;
  stopRightBack = false;

  go(-maxPower,maxPower);
  nr = -nr;
  while(!stopLeftFront || !stopLeftBack || !stopRightFront || !stopRightBack ) {
    if (BTSerial.available() > 1) {
      break;
    }
    if (!stopLeftFront && left_front_encoder_count >=  countRotate1Inner*nr) {
      stopLeftFront = true;
      breakLeftFrontEngine();
    }
    if (!stopRightFront && right_front_encoder_count >= countRotate1Outer*nr) {
      stopRightFront = true;
      breakRightFrontEngine();
   }
   if (!stopLeftBack && left_back_encoder_count >=  countRotate1Inner*nr) {
      stopLeftBack = true;
      breakLeftBackEngine();
   }
   if (!stopRightBack && right_back_encoder_count >= countRotate1Outer*nr) {
      stopRightBack = true;
      breakRightBackEngine();
   }
 }
 breakAllEngines();
  delay(100);  
  go(0,0);
}

void rotateRight(long nr) {
  resetCounters();
  stopLeftFront = false;
  stopRightFront = false;
  stopLeftBack = false;
  stopRightBack = false;

  go(maxPower,-maxPower);
  while(!stopLeftFront || !stopLeftBack || !stopRightFront || !stopRightBack ) {
    if (BTSerial.available() > 1) {
      break;
    }
    if (!stopLeftFront && left_front_encoder_count >= countRotate1Outer*nr) {
      stopLeftFront = true;
      breakLeftFrontEngine();
    }
    if (!stopRightFront && right_front_encoder_count >= countRotate1Inner*nr) {
      stopRightFront = true;
      breakRightFrontEngine();
    }
    if (!stopLeftBack && left_back_encoder_count >= countRotate1Outer*nr) {
      stopLeftBack = true;
      breakLeftBackEngine();
    }
    if (!stopRightBack && right_back_encoder_count >= countRotate1Inner*nr) {
      stopRightBack = true;
      breakRightBackEngine();
    }
  }
  breakAllEngines();
  delay(100);  
  go(0,0);
}

void restoreSafeDistance() {
  resetCounters();
  uint16_t distance = readDistance();
  if (distance < SAFE_DISTANCE) {
    moveBacwardLinear(-((SAFE_DISTANCE - distance)/10.0));    
  }
}

void rotate_move(long nr, float distance)
{
  resetCounters();
   pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
  if (nr < 0) {    
    rotateLeft(nr);
    while(readDistance() < distance*10) {
      if (BTSerial.available() > 1) {
        break;
      }
      rotateLeft(nr);
    }
  } else if (nr > 0) {
    rotateRight(nr);
    while(readDistance() < distance*10) {
      if (BTSerial.available() > 1) {
        break;
      }
      rotateRight(nr);
   }
  }  
  breakAllEngines();
  delay(100);  
  go(0,0);
  pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
  moveForwardLinear(distance);
}

void rotate_safe(long nr) {
  resetCounters();
  if (nr < 0) {
    pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_LEFT_45);
    delay(SERVO_LEFT_45);
    if (readDistance() > (SAFE_DISTANCE + 20)) {
      rotateLeft(nr);
    } else {
      rotateLeft(nr);
      if (readDistance() > (SAFE_DISTANCE + 20)) {
        rotateLeft(nr);
      } 
    }
  } else if (nr > 0) {
    pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_RIGHT_45);
    delay(SERVO_RIGHT_45);
    if (readDistance() > (SAFE_DISTANCE + 20)) {
      rotateRight(nr);
    } else {
      rotateRight(nr);
      if (readDistance() > (SAFE_DISTANCE + 20)) {
        rotateRight(nr);
      }
    }
  } else {
    return;
  }  
  breakAllEngines();
  delay(100);  
  go(0,0);
  pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);
  delay(SERVO_CENTER);
  restoreSafeDistance();  
}

void breakLeftFrontEngine() {
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 4095);
}

void breakLeftBackEngine() {
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 4095);
}

void stopLeftEngines() {
  breakLeftFrontEngine();
  breakLeftBackEngine();
}

void breakRightFrontEngine() {
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 4095);
}

void breakRightBackEngine() {
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 4095);
  pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 4095);
}
void stopRightEngines() {
  breakRightFrontEngine();
  breakRightBackEngine();
}
void breakAllEngines() {
  stopLeftEngines();
  stopRightEngines();
}
/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
#ifdef BLE_DEBUG_MODE
    BTSerial.print("leftPower=");BTSerial.print(speedLeft);
    BTSerial.print(" rightPower=");BTSerial.println(speedRight);
#endif
  if (speedLeft == 0 && speedRight == 0 ) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
    return;
  }
  if (speedLeft > 0) {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, speedLeft);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, 0);
  } 
  else {
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_FRONT_MOTOR_PIN2, 0, -speedLeft);    
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(LEFT_BACK_MOTOR_PIN2, 0, -speedLeft);    
  }
 
  if (speedRight > 0) {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, 0);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, speedRight);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, 0);
  }else {
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN1, 0, 0);    
    pwmDriver.setPWM(RIGHT_FRONT_MOTOR_PIN2, 0, -speedRight);    
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN1, 0, 0);
    pwmDriver.setPWM(RIGHT_BACK_MOTOR_PIN2, 0, -speedRight);
  }
}
static const char *pMenu = "MENU:";
static const char *pStop_Menu = "s# Stop";
static const char *pForwardMenu = "fxx# move forward xx cm";
static const char *pBackMenu = "bxx# backward xx cm";
static const char *pLeftMenu = "l# left 45 degree check distance and safe";
static const char *pRightMenu = "r# right right 45 degree check distance and safe";
static const char *pPrintClearEncoders = "c# print and clear encoders";
static const char *pChangePower = "vxx# change maxPower";
static const char *pDegree45 = "dxx# change rotation of platform";
static const char *pLeftMoveMenu = "lxx# left and move";
static const char *pRightMoveMenu = "rxx# right and move";
static const char *pSlowPowerMenu = "sxx# change slow power";
static const char *pEnablePixy2Menu = "p# enable/disable pixy2 camera";
static const char *pEnablePixy2LampMenu = "n# enable/disable lights";
static const char *pPixy2ServoPositionMenu = "pxx,yy# camera position";
void printMenu() {
    BTSerial.println( pMenu);
    BTSerial.println( pStop_Menu);
    BTSerial.println( pForwardMenu);
    BTSerial.println( pBackMenu);
    BTSerial.println( pLeftMenu);
    BTSerial.println( pLeftMoveMenu);
    BTSerial.println( pRightMenu);
    BTSerial.println( pRightMoveMenu);
    BTSerial.println( pPrintClearEncoders);
    BTSerial.println( pChangePower);
    BTSerial.println( pSlowPowerMenu);
    BTSerial.println( pDegree45);    
    BTSerial.println( pEnablePixy2Menu);
    BTSerial.println( pEnablePixy2LampMenu);
    BTSerial.println( pPixy2ServoPositionMenu);
}

void setup()
{
    //init the PWM driver
    Wire.begin();
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);    
    pixy.init();
    //enable BLE
    BTSerial.begin(38400);
    enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
    isValidInput = false;
    //enable encoders ISR
    pinMode(LEFT_FRONT_ENCODER_PIN, INPUT_PULLUP);
    pinMode(RIGHT_FRONT_ENCODER_PIN, INPUT_PULLUP);
    pinMode(LEFT_BACK_ENCODER_PIN, INPUT_PULLUP);
    pinMode(RIGHT_BACK_ENCODER_PIN, INPUT_PULLUP);
    enableInterrupt(LEFT_FRONT_ENCODER_PIN, isrLeftFrontEncoder, RISING);
    enableInterrupt(RIGHT_FRONT_ENCODER_PIN, isrRightFrontEncoder, RISING);
    enableInterrupt(LEFT_BACK_ENCODER_PIN, isrLeftBackEncoder, RISING);
    enableInterrupt(RIGHT_BACK_ENCODER_PIN, isrRightBackEncoder, RISING);
    printMenu();
    cleanupBT = false;
    maxPower=4000;
    frontSensor.init();
    //frontSensor.setSignalRateLimit(0.1);
    frontSensor.setTimeout(100);
    //frontSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    //frontSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    //frontSensor.setMeasurementTimingBudget(20000);
    frontSensor.startContinuous();
    //put the front servo and camera to center
    pwmDriver.setPWM(FRONT_SERVO_PIN, 0, SERVO_CENTER);    
    pixy.setServos(500,500);
    pixy.setLamp(0,0);
}

boolean isValidNumber(char *data, int size)
{
  if (size == 0 ) return false;
   boolean isNum=false;
   if(!(data[0] == '+' || data[0] == '-' || isDigit(data[0]))) return false;

   for(byte i=1;i<size;i++)
   {
       if(!(isDigit(data[i]) || data[i] == '.')) return false;
   }
   return true;
}

boolean makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
  BTSerial.flush();
}

void resetCounters() {
  left_front_encoder_count = 0;
  right_front_encoder_count = 0;
  left_back_encoder_count = 0;
  right_back_encoder_count = 0;
}

void printFrom() {
   BTSerial.print(" from ");
}
boolean makeMove() {
     if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strlen(inData) == 1) {
      if (inData[0] == 'c') {
        BTSerial.print("Left Front=");BTSerial.print(left_front_encoder_count);
        BTSerial.print("Left Back=");BTSerial.print(left_back_encoder_count);
        BTSerial.print("Right Front=");BTSerial.print(right_front_encoder_count);
        BTSerial.print("Right Back=");BTSerial.println(right_back_encoder_count);
        resetCounters();
      } else if (inData[0] == 's') {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("stop");
#endif
        breakAllEngines();
        delay(10);
        go(0, 0);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'l' ) {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("rotate left 45");
#endif
        rotate_safe(-left_right_45_encoders);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'r') {
#ifdef BLE_DEBUG_MODE      
        BTSerial.println("rotate right 45");
#endif
        rotate_safe(left_right_45_encoders);
        makeCleanup();        
        isValidInput = true;
     } else if (inData[0] == 'p') {
        enablePixy2 = !enablePixy2;
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Pixy2 enable/disable ");
        BTSerial.println(enablePixy2);
#endif
        if(enablePixy2) {          
          pixy.changeProg("line");
          pixy.setServos(hPixyServo,vPixyServo);
        } else {
          pixy.setLamp(0,0);
          pixy.setServos(500,500);
        }
        isValidInput = true;
     } else if (inData[0] == 'n') {
        enablePixy2Lamp = !enablePixy2Lamp;
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Pixy2 Lamps enable/disable ");
        BTSerial.println(enablePixy2Lamp);
#endif        
        isValidInput = true;
     }
    } else if (strlen(inData) > 1) {
        if (inData[0] == 'f') {
        //remove f from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving forward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif
        moveForwardLinear(atof(inData));
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving backward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif
        moveBacwardLinear(-atof(inData));
        makeCleanup();
        isValidInput = true;
      }  else if (inData[0] == 'l') {
        //remove l from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving left ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif        
        rotate_move(-left_right_45_encoders, atof(inData));
        makeCleanup();
        isValidInput = true;
      }  else if (inData[0] == 'r') {
        //remove r from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          return false;
        }
#ifdef BLE_DEBUG_MODE
        BTSerial.print("Moving right ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif
        rotate_move(left_right_45_encoders, atof(inData));
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'v') {
        //remove v from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(inData) > 4095 || atol(inData) < 0) {
          BTSerial.println("Speed value should be between 0 and 4095");
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(maxPower);
#endif
        maxPower = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'd') {
        //remove d from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change rotation value ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(left_right_45_encoders);
#endif
        left_right_45_encoders = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 's') {
        //remove s from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_SETTINGS_MODE        
        BTSerial.print("Change slowPower ");
        BTSerial.print(atol(inData));
        printFrom();
        BTSerial.println(slowPower);
#endif
        slowPower = atol(inData);
        makeCleanup();
        isValidInput = true;
      } else if (inData[0] == 'p') {
        //remove p from command
        for (uint8_t i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        char hposition[5];
        char vposition[5];
        for (uint8_t i = 0; i < 5; i++) {
          hposition[i] ='\0';
          vposition[i] = '\0';
        }
        boolean foundComa = false;
        uint8_t j = 0;
        uint8_t k = 0;
        for(uint8_t i = 0; i < strlen(inData) &&  j< 5 && k < 5; i++) {
          if ( inData[i] == ',') {
            foundComa = true;
            continue;
          }
          if (!foundComa) {
            hposition[j] = inData[i];
            j++;
          }
          if (foundComa) {
            vposition[k] = inData[i];
            k++;
          }
        }
        if (isValidNumber(hposition,j) && isValidNumber(vposition,k)) {
#ifdef BLE_DEBUG_SETTINGS_MODE        
          BTSerial.print("Change Pixy2 servos h=");
          BTSerial.print(atoi(hposition));
          printFrom();
          BTSerial.print(hPixyServo);
          BTSerial.print(" v=");
          BTSerial.print(atoi(vposition));
          printFrom();
          BTSerial.println(vPixyServo);
#endif
          hPixyServo = atoi(hposition);
          vPixyServo = atoi(vposition);
          pixy.setServos(hPixyServo, vPixyServo);
        } else {
          makeCleanup();
          printMenu();
          isValidInput = false;
          return false;
        }
      }
    } else {
      makeCleanup();
      printMenu();
      isValidInput = false;
    }
    makeCleanup();
    return true;
}

void loop()
{

  while(BTSerial.available() > 0) // Don't read unless there you know there is data
  {
     if(index < 19) // One less than the size of the array
     {
        inChar = BTSerial.read(); // Read a character
        inData[index] = inChar; // Store it
        index++; // Increment where to write next
        inData[index] = '\0'; // Null terminate the string
     } else {
        makeCleanup();
        cleanupBT = true;
    }
  }
  if (inChar == '#') {
     makeMove();
     return;
  } else if (cleanupBT) {
     makeCleanup();
     cleanupBT = false;
     printMenu();
  }
    delay(5);
}
