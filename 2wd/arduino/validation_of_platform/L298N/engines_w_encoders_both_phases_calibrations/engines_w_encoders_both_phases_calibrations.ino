/*
 * Engine with encoders both phases with driver L298N calibration.
 * Copyright 2019 Gabriel Dimitriu
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
#include <EnableInterrupt.h>
#define LEFT_MOTOR_PIN1 7
#define LEFT_MOTOR_PIN2 8
#define LEFT_MOTOR_SPEED 9
#define RIGHT_MOTOR_PIN1 4
#define RIGHT_MOTOR_PIN2 6
#define RIGHT_MOTOR_SPEED 5
#define LEFT_ENCODER_A 3
#define LEFT_ENCODER_B 10
#define RIGHT_ENCODER_A 12
#define RIGHT_ENCODER_B 13
float whellRadius = 3;
long resolutionCodor_left = 1452;
long resolutionCodor_right = 1468;
long resolutionCodor = 1468;
const float pi = 3.14f;
float PPI = resolutionCodor/(2*pi*whellRadius);
long countRotate90Left= 841;
long countRotate90Right= 830;
long countRotate1Inner = 9;
long countRotate1Outer = 10;

bool isValidInput;
char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
volatile long left_encoder_A_count = 0;
volatile long left_encoder_B_count = 0;
volatile long right_encoder_A_count = 0;
volatile long right_encoder_B_count = 0;
float currentPosition = 0.0f;

/*
 * isr for encoder pins
 */
void isrLeftEncoderA() {
  left_encoder_A_count++;
}

void isrLeftEncoderB() {
  left_encoder_B_count++;
}

void isrRightEncoderA() {
  right_encoder_A_count++;
}

void isrRightEncoderB() {
  right_encoder_B_count++;
}

void setup() {
  Serial.begin(9600);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_SPEED, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(RIGHT_MOTOR_SPEED, OUTPUT);
  pinMode(LEFT_ENCODER_A, INPUT_PULLUP);
  pinMode(LEFT_ENCODER_B, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER_A, INPUT_PULLUP);
  pinMode(RIGHT_ENCODER_B, INPUT_PULLUP);
  isValidInput = false;
  enableInterrupt(LEFT_ENCODER_A, isrLeftEncoderA, RISING);
  enableInterrupt(LEFT_ENCODER_B, isrLeftEncoderB, RISING);
  enableInterrupt(RIGHT_ENCODER_A, isrRightEncoderA, RISING);
  enableInterrupt(RIGHT_ENCODER_B, isrRightEncoderB, RISING);
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
  left_encoder_A_count = 0;
  left_encoder_B_count = 0;
  right_encoder_A_count = 0;
  right_encoder_B_count = 0;
}

void loop() {
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "Calibration of engines with encoder both phases using driver L298N");
  Serial.println( "------------------------------------------------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "s# Stop" );
  Serial.println( "p# print encoder counting");
  Serial.println( "c# reset counters");
  Serial.println( "f# forward until stop command");
  Serial.println( "fxx# move forward xx cm");
  Serial.println( "bxx# move backward xx cm");
  Serial.println( "l# rotate 90 left");
  Serial.println( "r# rotate 90 right");
  Serial.println( "lxx# rotate xx degree left");
  Serial.println( "rxx# rotate xx degree right");

  Serial.println( "-----------------------------" );
   do {
    for (index = 0; index < 20; index++)
    {
       inData[index] = '\0';
    }
    inChar = '0';
    index = 0;
    while(inChar != '#') {
      while( !Serial.available() )
        ; // LOOP...
      while(Serial.available() > 0) // Don't read unless
                                                 // there you know there is data
      {
          if(index < 19) // One less than the size of the array
          {
              inChar = Serial.read(); // Read a character
              inData[index] = inChar; // Store it
              index++; // Increment where to write next
              inData[index] = '\0'; // Null terminate the string
          }
      }
    }
    if (index > 0) {
      inData[index-1] = '\0';
    }
    if (strcmp(inData,"f") == 0) {
      Serial.println("forward");
      go(255, 255);
      delay(1000);
      isValidInput = true;
    } else if (strcmp(inData,"s") == 0) {
      Serial.println("stop");
      go(0, 0);
      isValidInput = true;
    } else if (strcmp(inData,"c") == 0) {
      Serial.println("reset counters");
      resetCounters();
      isValidInput = true;
    } else if (strcmp(inData,"l") == 0 ) {
      Serial.println("rotate 90 degree left");
      rotate90Left();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
      Serial.println("rotate 90 degree right");
      rotate90Right();
      isValidInput = true;
    } else if (strcmp(inData,"p") == 0 ) {
        Serial.print("EncoderValues leftA="); Serial.print(left_encoder_A_count);
        Serial.print(" leftB=");Serial.print(left_encoder_B_count);
        Serial.print(" rightA=");Serial.print(right_encoder_A_count);
        Serial.print(" rightB=");Serial.println(right_encoder_B_count);
        isValidInput = true;
    } else if (strlen(inData) > 1) {
      if (inData[0] == 'f') {
        //remove f from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Moving forward ");
        Serial.print(atof(inData));
        Serial.println(" cm");
        moveForward(atof(inData));
        isValidInput = true;
      } else if (inData[0] == 'l') {
          //remove l from command
          for (int i = 0 ; i < strlen(inData); i++) {
            inData[i]=inData[i+1];
          }
          if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Rotate left  ");
        Serial.print(atol(inData));
        Serial.println(" degree");
        rotateLeftDegree(atol(inData));
        isValidInput = true;
      } else if (inData[0] == 'r') {
        //remove r from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
         isValidInput = false;
          break;
        }
        Serial.print("Rotate right  ");
        Serial.print(atol(inData));
        Serial.println(" degree");
        rotateRightDegree(atol(inData));
        isValidInput = true;
      } else if (inData[0] == 'b') {
        //remove b from command
        for (int i = 0 ; i < strlen(inData); i++) {
          inData[i]=inData[i+1];
        }
        if (!isValidNumber(inData, index - 2)) {
          isValidInput = false;
          break;
        }
        Serial.print("Moving backward ");
        Serial.print(atof(inData));
        Serial.println(" cm");
        moveBackward(atof(inData));
        isValidInput = true;
      } else {
        isValidInput = false;
      }
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}

void go(int speedLeft, int speedRight) {
  if (speedLeft == 0 && speedRight == 0 ) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, LOW);
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, LOW);
    return;
  }
  if (speedLeft > 0) {
    digitalWrite(LEFT_MOTOR_PIN1,LOW);
    digitalWrite(LEFT_MOTOR_PIN2,HIGH);
    analogWrite(LEFT_MOTOR_SPEED, speedLeft);
  } 
  else {
    digitalWrite(LEFT_MOTOR_PIN1,HIGH);
    digitalWrite(LEFT_MOTOR_PIN2,LOW);
    analogWrite(LEFT_MOTOR_SPEED, -speedLeft);
  }
 
  if (speedRight > 0) {
    digitalWrite(RIGHT_MOTOR_PIN1,LOW);
    digitalWrite(RIGHT_MOTOR_PIN2,HIGH);
    analogWrite(RIGHT_MOTOR_SPEED, speedRight);
  }else {
    digitalWrite(RIGHT_MOTOR_PIN1,HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2,LOW);
    analogWrite(RIGHT_MOTOR_SPEED, -speedRight);
  }
}

void rotate90Left() {
  go(-255,255);
  while(left_encoder_A_count < countRotate90Left && right_encoder_A_count < countRotate90Right) {
    
  }
  go(0,0);
}

void rotateLeftDegree(long nr) {
  go(-255,255);
  while(left_encoder_A_count < countRotate1Inner*nr && right_encoder_A_count < countRotate1Outer*nr) {
    
  }
  go(0,0);
}

void rotateRightDegree(long nr) {
  go(255,-255);
  while(left_encoder_A_count < countRotate1Outer*nr && right_encoder_A_count < countRotate1Inner*nr) {
    
  }
  go(0,0);
}

void rotate90Right() {
  go(255,-255);
  while(left_encoder_A_count < countRotate90Left && right_encoder_A_count < countRotate90Right) {
    
  }
  go(0,0);
  
}

void moveForward(float distance) {
  go(255,255);
  while(!((distance - currentPosition) < 0.2)){
    currentPosition = left_encoder_A_count/PPI;
  }
  go(0,0);
  Serial.print("Current position=");
  Serial.println(distance);
  currentPosition = 0;
}

void moveBackward(float distance) {
  go(-255,-255);
  while(!((distance - currentPosition) < 0.2)){
    currentPosition = left_encoder_A_count/PPI;
  }
  go(0,0);
  Serial.print("Current position=");
  Serial.println(distance);
  currentPosition = 0;
}
