/*
 * Copyright 2018 Gabriel Dimitriu

 * This file is part of Robotics project.

 * Robotics is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * Robotics is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with Robotics; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "AutonomousDroid.h"
#include "SweepSensorHCSR04.h"
#include "AutonomousDroidSettings.h"
#include "SensorsSubSystem.h"
#include "EnginesSubSystemL298.h"
#include "NavigationSubSystem.h"
#include "RechargeSubSystem.h"
#include "ArduinoI2CSubSystem.h"
#include "ArduinoKeypadCommandSubSystem.h"
#include "I2CSubSystem.h"
#include "ArduinoI2CSubSystem.h"
#include "CommandInterpreter.h"

#define LEFT_MOTOR_PIN1 6
#define LEFT_MOTOR_PIN2 5
#define LEFT_MOTOR_PIN3 4
#define RIGHT_MOTOR_PIN1 7
#define RIGHT_MOTOR_PIN2 8
#define RIGHT_MOTOR_PIN3 9

#define LEFT_SENSOR_PIN 25
#define CENTRAL_SENSOR_PIN 23
#define RIGHT_SENSOR_PIN 24

#define ECHO_PIN 10
#define TRIG_PIN 11
#define SENSOR_SERVO_PIN 12

#define SENSOR_BACKWARD_PIN 22
#define RADAR_WAKEUP_PIN 2
#define COMMAND_BUTTON_PIN 19

#define MIN_DISTANCE 200
#define MAX_DISTANCE 4000
#define STOP_DISTANCE 100
#define DEBUG_MODE true

#define LCD_ROWS 4
#define LCD_COLS 20
#define LCD_ADDRESS 0x3F


#define KEYPAD_ROWS 4 //four rows
#define KEYPAD_COLS 4 //four columns
#define DEBUG_MODE true
#define LCD_ADDRESS 0x3F

char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte colPins[KEYPAD_ROWS] = {31,30,29,28}; //connect to the row pinouts of the keypad (31,30,29,28) vs orig {6,5,4,3}
byte rowPins[KEYPAD_COLS] = {35,34,33,32}; //connect to the column pinouts of the keypad (35,34,33,32) vs orig {11, 10, 9,8 }


AutonomousDroid *droid;
AutonomousDroidSettings *settings;
I2CSubSystem *i2cBus;
SweepSensorHCSR04 *frontSweepSensor;
SensorsSubSystem *sensors;
EnginesSubSystemL298 *engines;
NavigationSubSystem *navigation;
RechargeSubSystem *recharge;
ArduinoKeypadCommandSubSystem *commandSubSystem;
ArduinoKeypadSettings * keypadSettings;


bool stopFlag = false;

void setup() {
  Serial.begin(9600);
  droid = new AutonomousDroid();
  droid->setDebugMode(DEBUG_MODE);
  //settings
  settings = new AutonomousDroidSettings();
  settings->setMaxDistance(MAX_DISTANCE);
  settings->setMinDistance(MIN_DISTANCE);
  settings->setTiming90Left(800);
  settings->setTiming90Right(800);
  settings->setTimingSlowForward(100);
  settings->setTiming1Degree(10);
  droid->setSettings(settings);
  //i2cBus
  i2cBus = new ArduinoI2CSubSystem(0x01);
  i2cBus->addLCDDevice(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
  i2cBus->addMagneticCompassAddress(0x1E);
  droid->setI2CBus(i2cBus);
  //sensors
  sensors = new SensorsSubSystem();
  sensors->attachFrontLeftSensor(LEFT_SENSOR_PIN, LOW);
  sensors->attachFrontCentralSensor(CENTRAL_SENSOR_PIN, LOW);
  sensors->attachFrontRightSensor(RIGHT_SENSOR_PIN, LOW);
  frontSweepSensor = new SweepSensorHCSR04(TRIG_PIN, ECHO_PIN);
  sensors->attachFrontSweepSensor(SENSOR_SERVO_PIN, frontSweepSensor);
  sensors->attachReverseSensor(SENSOR_BACKWARD_PIN, LOW);
  droid->setSensorsSubSystem(sensors);
  //engines
  engines = new EnginesSubSystemL298();
  engines->addLeftMotorSettings(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, LEFT_MOTOR_PIN3);
  engines->addRightMotorSettings(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, RIGHT_MOTOR_PIN3);
  droid->setEnginesSubSystem(engines);
  //navigation
  navigation = new NavigationSubSystem();
  droid->setNavigationSubSystem(navigation);
  keypadSettings = new ArduinoKeypadSettings(KEYPAD_ROWS,KEYPAD_COLS);
  keypadSettings->setKeyMap(makeKeymap(keys));
  keypadSettings->setRowPins(rowPins);
  keypadSettings->setColPins(colPins);
  
  keypadSettings->setCommandButton(COMMAND_BUTTON_PIN);
  commandSubSystem = new ArduinoKeypadCommandSubSystem(i2cBus, keypadSettings); 
  commandSubSystem->setCommandInterpreter(new CommandInterpreter());
  commandSubSystem->setAutonomousDroidSettings(settings);
  commandSubSystem->setNavigationSubSystem(navigation);
  droid->setCommandSubSystem(commandSubSystem);
  //recharge
/*  recharge = new RechargeSubSystem();
  recharge->addPhotoSensors(A0, A1);
  recharge->setWakeUpSensor(RADAR_WAKEUP_PIN, HIGH);
  droid->setRechargeSubSystem(recharge); */
  if (droid->setup()) {
    Serial.println("all initialized");
    stopFlag = false;
  } else {
    Serial.println("engine or sensors are missing");
    stopFlag = true;
  }
}

void loop() {
  if (!stopFlag) {
    droid->move();
  }
}
