/*
 * 4 whell droid controled by bluetooth.
 * this is my version of the droid from "Introducere in Arduino"
 * by Optimnus Digital
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
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NeoSWSerial.h>
#include <EnableInterrupt.h>

#define RxD 2
#define TxD 3

#define SELECT_CONSOLE 0
#define DEBUG_MODE 0
#define AUTO_TIME 40000
#define BLE_DEBUG_MODE true

#define trigPinR 5
#define echoPinR 6
#define trigPinL 10
#define echoPinL 11

#define setDistance 20

int LEFT_FRONT_MOTOR_PIN1 = 2;
int LEFT_FRONT_MOTOR_PIN2 = 3;
int RIGHT_FRONT_MOTOR_PIN1 = 0;
int RIGHT_FRONT_MOTOR_PIN2 = 1;

int LEFT_BACK_MOTOR_PIN1 = 13;
int LEFT_BACK_MOTOR_PIN2 = 12;
int RIGHT_BACK_MOTOR_PIN1 = 15;
int RIGHT_BACK_MOTOR_PIN2 = 14;
int maxPower = 4000;
Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);

// reține timpul de rulare a programului
long dt;

int turatie; // controlează turația motoarelor
bool sens;   // controlează sensul motoarelor
int btCommand;
int mode = NULL;
bool lastDirection = true;

// variables used for PID algorithm
int lastError = 0;
int error;
float Kp, Ki, Kd;
int delayMove;

NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
}

long read(bool side)
{
    long durata, distanta;
    int trigPin, echoPin;
    if (side) { 
        // RIGHT SENSOR
        trigPin = trigPinR;
        echoPin = echoPinR;
    }
    else { 
        // LEFT SENSOR
        trigPin = trigPinL;
        echoPin = echoPinL;
    }

    // secvența inițială
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    // se emite o undă
    digitalWrite(trigPin, LOW);

    // se măsoară durata în care unda este recepționată
    durata = pulseIn(echoPin, HIGH);

    // se calculează distanța
    // (se cunoaște viteza de propagare a undei emise)
    distanta = (durata / 2) / 29.1;

    return distanta;
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
/*
 * 
 #ifdef BLE_DEBUG_MODE
    BTSerial.print("left=");BTSerial.print(speedLeft);
    BTSerial.print(" right=");BTSerial.println(speedRight);
#endif
*/
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

void followObjectPID(long left, long right)
{
    long avg = (right + left) / 2;
    int P, I, D;
    if (left - right > 10) {
        // turn right
        go(maxPower/2,-maxPower/2);
        delay(delayMove);

        // pentru stabilitate, oprește motoarele
        // la fiecare curbă mică
        go(0,0);
    }
    else if (right - left > 10) {
        // turn left
        go(-maxPower/2,maxPower);
        delay(delayMove);

        // pentru stabilitate, oprește motoarele
        // la fiecare curbă mică
        go(0,0);
    }
    else {
        // PID control
        error = avg - setDistance;
        P = error;

        if (error > 80) {
#ifdef BLE_DEBUG_MODE
            BTSerial.println("No object in front of the robot");
#endif
            Serial.println("No object in front of the robot");
        }
        else {
            I += error;
            if (I > maxPower)
                I = maxPower;
            if (I < -maxPower)
                I = -maxPower;

            D = error - lastError;
            if (D > maxPower)
                D = maxPower;
            if (D < -maxPower)
                D = -maxPower;

            lastError = error;

            int command = Kp * P + Ki * I + Kd * D;

            // Mesaje posibil utile pentru debug / reglare
            if (DEBUG_MODE) {
                Serial.print("Comanda: ");
                Serial.println(command);
                Serial.print("Integrator: ");
                Serial.println(I);
                Serial.print("Eroare: ");
                Serial.println(error);
            }
#ifdef BLE_DEBUG_MODE    
            BTSerial.print("Comanda: ");
            BTSerial.println(command);
            BTSerial.print("Integrator: ");
            BTSerial.println(I);
            BTSerial.print("Eroare: ");
            BTSerial.println(error);
#endif
            if (command >= 0) {
                if (command > maxPower)
                    command = maxPower;
                go(command,command);
                delay(5);
            }
            if (command < 0) {
                if (command < -maxPower)
                    command = maxPower;
                go(-command, - command);
                delay(5);
            }
            if (abs(error) < 3) {
                // dacă distanța este între 17 și 23
                // atunci oprește motoare
                go(0,0);
            }
        }
    }
}

void setup()
{
    // inițializare comunicație serială
    Serial.begin(9600);
    //init the PWM driver
    pwmDriver.begin();
    pwmDriver.setOscillatorFrequency(27000000);
    pwmDriver.setPWMFreq(50.0);
    Wire.setClock(400000);
    BTSerial.begin(38400);
    pinMode(echoPinL, INPUT); // undă recepționată
    pinMode(echoPinR, INPUT); // undă recepționată
    pinMode(trigPinL, OUTPUT); // undă emisă
    pinMode(trigPinR, OUTPUT); // undă emisă    
    enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
    BTSerial.println("Starting...");
    if (SELECT_CONSOLE) {
        Serial.println("");
        Serial.println("Initializing services...");
        Serial.println("---------------------------------");
        Serial.println("Command list");
        Serial.println(" 7 - Automous Mode");
        Serial.println(" 9 - Manual Mode");
        Serial.println(" 0 - Switch Mode");
        Serial.println("\n\r In Manual Mode:");
        Serial.println(" 8 - Forward");
        Serial.println(" 4 - Left");
        Serial.println(" 6 - Right");
        Serial.println(" 5 - Backwards");
        Serial.println("---------------------------------");
    }
    else {
        BTSerial.write("");
        BTSerial.write("Initializing services...");
        BTSerial.write("---------------------------------");
        BTSerial.write("Command list");
        BTSerial.write(" 7 - Automous Mode");
        BTSerial.write(" 9 - Manual Mode");
        BTSerial.write(" 0 - Switch Mode");
        BTSerial.write("\n\r In Manual Mode:");
        BTSerial.write(" 8 - Forward");
        BTSerial.write(" 4 - Left");
        BTSerial.write(" 6 - Right");
        BTSerial.write(" 5 - Backwards");
        BTSerial.write("---------------------------------");
    }

    //Tune PID
    Kp = 500 ;
    Kd = 0;
    Ki = 0.2;
    delayMove = 50;
}

void loop()
{
    if (BTSerial.available()) {
        if (mode == NULL) {
            mode = BTSerial.read();
            Serial.println("Starting...");
        }

        if (DEBUG_MODE)
            Serial.println(btCommand);

        if (mode == '9') {
            // Modul manual
            btCommand = BTSerial.read();
            switch (btCommand) {
            case '8': // Inainte
            {
                go(maxPower,maxPower);
                lastDirection = true;
                delay(delayMove);
                break;
            }
            case '5': // Inapoi
            {
                go(-maxPower,-maxPower);
                lastDirection = false;
                delay(delayMove);
                break;
            }
            case '6': // Dreapta
            { //motorul stâng va avea viteză mai mare
                go(maxPower,-maxPower);
                delay(delayMove);
                break;
            }
            case '4': // Dreapta
            {   // motorul drept va avea viteză mai mare
                go(-maxPower,maxPower);
                delay(delayMove);
                break;
            }
            case '0': // Schimbă modul de rulare
            {
                mode = '7';
                Serial.println("Autonomous MODE");
                BTSerial.flush();
                break;
            }
            default: { // Stop
                go(0,0);
              }
            }
        }
        else if (mode == '7') {
            // Modul Autonom
            btCommand = BTSerial.read();

            if (btCommand == '0') {
                mode = '9';
                BTSerial.println("Manual MODE");
            }
            else {
                dt = millis();

                while ((millis() - dt) < AUTO_TIME) {
                    long distanceR, distanceL;
                    distanceR = read(1);
                    BTSerial.print("Right:  ");
                    BTSerial.print(distanceR);
                    BTSerial.print(" cm");
                    delay(200);
                    distanceL = read(0);
                    BTSerial.print("  Left:  ");
                    BTSerial.print(distanceL);
                    BTSerial.println(" cm");

                    followObjectPID(distanceL, distanceR);
                }
                dt = millis();
                BTSerial.flush();
                BTSerial.println("Press 0 to switch mode or any other key to continue...");
            }
        }
        else {
            BTSerial.println("Please choose a valid running configuration !");
            mode = BTSerial.read();
        }
    }
    else {
      go(0,0);
    }
    BTSerial.flush();
    delay(5);
}
