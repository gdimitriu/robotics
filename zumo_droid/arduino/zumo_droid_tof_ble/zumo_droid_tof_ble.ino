/*
 * Zumo droid with tof_ble for colision avoidance droid.
 * Engines using encoder phase A and MX1508 driver.
 *
 * Copyright 2021 Gabriel Dimitriu
 *
 * This file is part of Robotics project.

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
//#define BLE_DEBUG_MODE true

#include <VL53L0X.h>
#include <Wire.h>
#include <EnableInterrupt.h>
#include <NeoSWSerial.h>

#define RxD 2
#define TxD 3

#define LEFT_MOTOR_PIN1 5
#define LEFT_MOTOR_PIN2 6
#define RIGHT_MOTOR_PIN1 10
#define RIGHT_MOTOR_PIN2 11
#define LEFT_ENCODER 4
#define RIGHT_ENCODER 12

float whellRadius = 2;
long resolutionCodor_left = 1452;
long resolutionCodor_right = 1468;
const float pi = 3.14f;
float PPI_left = resolutionCodor_left/(2*pi*whellRadius);
float PPI_right = resolutionCodor_right/(2*pi*whellRadius);
long countRotate90Left= 1030;//980;
long countRotate90Right= 1040;//990;
long countRotate1Inner = 12;
long countRotate1Outer = 15;

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
volatile long left_encoder_count = 0;
volatile long right_encoder_count = 0;
float currentLeftPosition = 0.0f;
float currentRightPosition = 0.0f;
// for input data from ble
boolean cleanupBT;

long speedValue = 255;
long followDistance = 100; //10cm
boolean followEnable = false;
uint16_t collisionDistance = 100;//10cm 
boolean enableAroundObject = false;
boolean isIncollision = false;
VL53L0X frontSensor;

NeoSWSerial BTSerial(RxD, TxD);

//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

#define SERIAL_DEBUG_MODE true

/*
 * isr for encoder pins
 */
void isrLeftEncoder() {
  left_encoder_count++;
}

void isrRightEncoder() {
  right_encoder_count++;
}

uint16_t readDistance() {
  return frontSensor.readRangeSingleMillimeters();
}

void printMenu() {
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "Zumo droid tof ble command");
  BTSerial.println( "-----------------------------------------------------" );
  BTSerial.println( "MENU:" );
  BTSerial.println( "s# Stop" );
  BTSerial.println( "p# print encoder counting");
  BTSerial.println( "C# reset counters");
  BTSerial.println( "h# print help");
  BTSerial.println( "fxx# move forward xx cm");
  BTSerial.println( "bxx# move backward xx cm");
  BTSerial.println( "l# rotate 90 left");
  BTSerial.println( "r# rotate 90 right");
  BTSerial.println( "lxx# rotate xx degree left");
  BTSerial.println( "rxx# rotate xx degree right");
  BTSerial.println( "vxx# change speed value");
  BTSerial.println( "Fxx# follow distance mm");
  BTSerial.println( "F# follow distance enable/disable");
  BTSerial.println( "Sxx# set stop (collision) distance");
  BTSerial.println( "c# enable around object");
  BTSerial.println( "-----------------------------" );
}

void setup() {
#ifdef SERIAL_DEBUG_MODE
  Serial.begin(9600);
  Serial.println("starting");
#endif
  Wire.begin();
  Wire.setClock(400000);    
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_ENCODER, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER, INPUT_PULLUP);
  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  isValidInput = false;
  enableInterrupt(LEFT_ENCODER, isrLeftEncoder, RISING);
  enableInterrupt(RIGHT_ENCODER, isrRightEncoder, RISING);
  cleanupBT = false;
  enableAroundObject = false;
  frontSensor.init();
  //frontSensor.setSignalRateLimit(0.1);
  frontSensor.setTimeout(100);
  //frontSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  //frontSensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  //frontSensor.setMeasurementTimingBudget(20000);
  //frontSensor.startContinuous();
  BTSerial.begin(38400);
  BTSerial.println("Starting...");
  printMenu();
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

void resetCounters() {
  left_encoder_count = 0;
  right_encoder_count = 0;
}

void makeCleanup() {
  for (index = 0; index < 20; index++)
  {
    inData[index] = '\0';
  }
  index = 0;
  inChar ='0';
}

void followFixDistance() {
  uint16_t readValue = readDistance();
  resetCounters();
  if (readValue > followDistance*2) {
    moveForward(followDistance/10);
    return;
  }
  if (readValue > followDistance) {
    moveForward((readValue - followDistance)/10);
  } else if (readValue < followDistance) {
    moveBackward((followDistance-readValue)/10);
  } else {
    go(0,0);
  }
}

boolean makeMove() {
    if (strcmp(inData,"s") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("stop");
#endif      
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"F") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("Follow");
#endif      
      followEnable = !followEnable;
      isValidInput = true;
    } else if (strcmp(inData,"c") == 0) {
      enableAroundObject = !enableAroundObject;
      isValidInput = true;
      if (enableAroundObject)
        BTSerial.println("Enabled Around Object");
      else
        BTSerial.println("Disabled Around Object");
    } else if (strcmp(inData,"s") == 0) {
      BTSerial.println("stop");
      go(0, 0);
      followEnable = false;
      isValidInput = true;
    } else if (strcmp(inData,"C") == 0) {
      BTSerial.println("reset counters");
      resetCounters();
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0 ) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate 90 degree left");
#endif      
      rotate90Left();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
#ifdef BLE_DEBUG_MODE      
      BTSerial.println("rotate 90 degree right");
#endif      
      rotate90Right();
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0 ) {
        BTSerial.print("EncoderValues left="); BTSerial.print(left_encoder_count);
        BTSerial.print(" right=");BTSerial.println(right_encoder_count);        
        isValidInput = true;
    } else if (strcmp(inData,"h") == 0) {
      printMenu();
      isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Moving forward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif  
        float remainDistance = atof(inData) - moveForward(atof(inData));
        delay(100);
        if (remainDistance > 0 && isIncollision && enableAroundObject) {
          remainDistance -=moveAroundObject();
        }
        delay(100);
        moveForward(remainDistance);
        isValidInput = true;
      } else if (inData[0] == 'l') {
          //remove l from command
          for (int i = 0 ; i < strlen(inData); i++) {
            inData[i]=inData[i+1];
          }
          if (!isValidNumber(inData, index - 2)) {
            isValidInput = false;
            makeCleanup();
            return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Rotate left  ");
        BTSerial.print(atol(inData));
        BTSerial.println(" degree");
#endif        
        rotateLeftDegree(atol(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Rotate right  ");
        BTSerial.print(atol(inData));
        BTSerial.println(" degree");
#endif        
        rotateRightDegree(atol(inData));
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef BLE_DEBUG_MODE        
        BTSerial.print("Moving backward ");
        BTSerial.print(atof(inData));
        BTSerial.println(" cm");
#endif        
        moveBackward(atof(inData));
        isValidInput = true;
      } else if (inData[0] == 'v') {
        //remove v from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(inData) > 255 || atol(inData) < 0) {
          BTSerial.println("Speed value should be between 0 and 255");
          isValidInput = false;
          makeCleanup(); 
          return false;
        }
        BTSerial.print("Change speed ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(speedValue);
        speedValue = atol(inData);
        isValidInput = true;
      } else if (inData[0] == 'F') {
        //remove F from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        BTSerial.print("Change follow distance ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(followDistance);
        followDistance = atol(inData);
        isValidInput = true;
      } else if (inData[0] == 'S') {
        //remove S from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        BTSerial.print("Change collision distance ");
        BTSerial.print(atol(inData));
        BTSerial.print(" from ");
        BTSerial.println(collisionDistance);
        followDistance = atol(inData);
        isValidInput = true;
      } else {
        isValidInput = false;
        makeCleanup(); 
        return false;
      }
    } else {
      makeCleanup(); 
      isValidInput = false;
      return false;
    }
    makeCleanup();
    return true;
}

void loop() {
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
     inData[index-1] = '\0';
     makeMove();
     return;
  } else if (cleanupBT) {
     makeCleanup();
     cleanupBT = false;
     printMenu();
  }
  if (followEnable) {
    followFixDistance();
  }
}

void go(int speedLeft, int speedRight) {  
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.println("All on zero");
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.println("All on zero");
#endif
    return;
  }
  if (speedLeft > 0) {
    analogWrite(LEFT_MOTOR_PIN1, speedLeft);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(speedLeft); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(speedLeft); BTSerial.print(" , "); BTSerial.println(0);
#endif
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    analogWrite(LEFT_MOTOR_PIN2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Left "); Serial.print(0); Serial.print(" , "); Serial.println(-speedLeft);
#endif    
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Left "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedLeft);
#endif
  }
 
  if (speedRight > 0) {
    analogWrite(RIGHT_MOTOR_PIN1, speedRight);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(speedRight); Serial.print(" , "); Serial.println(0);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(speedRight); BTSerial.print(" , "); BTSerial.println(0);
#endif
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    analogWrite(RIGHT_MOTOR_PIN2, -speedRight);
#ifdef SERIAL_DEBUG_MODE
    Serial.print("Right "); Serial.print(0); Serial.print(" , "); Serial.println(-speedRight);
#endif
#ifdef BLE_DEBUG_MODE
    BTSerial.print("Right "); BTSerial.print(0); BTSerial.print(" , "); BTSerial.println(-speedRight);
#endif
  }
}

float moveAroundObject() {
  float movedLinear;
  boolean isLeft =false;
  rotate90Left();
  delay(100);
  uint16_t leftDistance = readDistance();
  rotate90Right();
  delay(100);
  rotate90Right();
  uint16_t rightDistance = readDistance();
  if (leftDistance > rightDistance) {
    rotate90Left();
    delay(100);
    rotate90Left();
    delay(100);
    isLeft = true;
  } else {
    isLeft = false;
  }
  moveForward(15);//15cm
  delay(100);
  if (isLeft) {
    rotate90Right();
  } else {
    rotate90Left();
  }
  delay(100);
  moveForward(15+collisionDistance/10);//15cm
  delay(100);
  if (isLeft) {
    rotate90Right();
  } else {
    rotate90Left();
  }
  delay(100);
  moveForward(15);//10cm
  delay(100);
  if(isLeft) {
    rotate90Left();
  } else {
    rotate90Right();
  }
  delay(100);
  return 15 + collisionDistance/10;
}

void rotate90Left() {
  boolean leftStopped = false;
  boolean rightStopped = false;
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(-speedValue,speedValue);
  while(!rightStopped || !leftStopped) {
    if (left_encoder_count >= countRotate90Left && !leftStopped) {
      stopLeftEngine();
      leftStopped = true;
    }
    if (right_encoder_count >= countRotate90Right && !rightStopped) {
      stopRightEngine();
      rightStopped = true;
    }
  }
  go(0,0);
}

void rotate90Right() {
  boolean leftStopped = false;
  boolean rightStopped = false;
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(speedValue,-speedValue);
  while(!rightStopped || !leftStopped) {
    if (left_encoder_count >= countRotate90Left && !leftStopped) {
      stopLeftEngine();
      leftStopped = true;
    }
    if (right_encoder_count >= countRotate90Right && !rightStopped) {
      stopRightEngine();
      rightStopped = true;
    }
  }
  go(0,0);
}

void rotateLeftDegree(long nr) {
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(-speedValue,speedValue);
  while(left_encoder_count < countRotate1Inner*nr && right_encoder_count < countRotate1Outer*nr);
  go(0,0);
}

void rotateRightDegree(long nr) {
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(speedValue,-speedValue);
  while(left_encoder_count < countRotate1Outer*nr && right_encoder_count < countRotate1Inner*nr);
  go(0,0);
}


float moveForward(float distance) {
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(speedValue,speedValue);
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){
    if (collisionDistance >= readDistance()) {
      stopLeftEngine();
      stopRightEngine();
      stopLeft = true;
      stopRight=true;
      isIncollision = true;
    }
    if (!stopLeft) {
      if((distance - currentLeftPosition) > 0.2){
        currentLeftPosition = left_encoder_count/PPI_left;
      } else {
        stopLeftEngine();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightPosition) > 0.2) {
        currentRightPosition = right_encoder_count/PPI_right;
      } else {
        stopRightEngine();
        stopRight=true;
      }
    }
  }
  go(0,0);
#ifdef SERIAL_DEBUG_MODE
  Serial.print("Current position="); Serial.print(distance);
  Serial.print(" leftPosition="); Serial.print(currentLeftPosition);
  Serial.print(" rightPosition="); Serial.println(currentRightPosition);
#endif
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightPosition);
#endif
  float travelDistance = 0.0f;
  if (currentLeftPosition < currentRightPosition) {
    travelDistance = currentRightPosition; 
  } else {
    travelDistance = currentLeftPosition; 
  }
  currentLeftPosition = 0;
  currentRightPosition = 0;
  return travelDistance;
}

void moveBackward(float distance) {
  //go to idle and reset counters
  go(0,0);
  resetCounters();
  go(-speedValue,-speedValue);
  boolean stopLeft = false;
  boolean stopRight = false;
  while(!stopLeft || !stopRight){    
    if (!stopLeft) {
      if((distance - currentLeftPosition) > 0.2){
        currentLeftPosition = left_encoder_count/PPI_left;
      } else {
        stopLeftEngine();
        stopLeft = true;
      }
    }
    if (!stopRight) {
      if ((distance-currentRightPosition) > 0.2) {
        currentRightPosition = right_encoder_count/PPI_right;
      } else {
        stopRightEngine();
        stopRight=true;
      }
    }
  }
  go(0,0);
#ifdef SERIAL_DEBUG_MODE
  Serial.print("Current position="); Serial.print(distance);
  Serial.print(" leftPosition="); Serial.print(currentLeftPosition);
  Serial.print(" rightPosition="); Serial.println(currentRightPosition);
#endif
#ifdef BLE_DEBUG_MODE
  BTSerial.print("Current position="); BTSerial.print(distance);
  BTSerial.print(" leftPosition="); BTSerial.print(currentLeftPosition);
  BTSerial.print(" rightPosition="); BTSerial.println(currentRightPosition);
#endif
  currentLeftPosition = 0;
  currentRightPosition = 0;
}

void stopLeftEngine() {
    digitalWrite(LEFT_MOTOR_PIN1,HIGH);
    digitalWrite(LEFT_MOTOR_PIN2,HIGH);
}

void stopRightEngine() {
    digitalWrite(RIGHT_MOTOR_PIN1,HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2,HIGH);
}
