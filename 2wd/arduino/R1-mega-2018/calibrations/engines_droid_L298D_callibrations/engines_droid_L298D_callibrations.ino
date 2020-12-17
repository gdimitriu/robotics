/*
  Copyright 2018 Gabriel Dimitriu

  This file is part of Autonomous Droid

  Autonomous Droid is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  Autonomous Droid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with Autonomous Droid; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <AutonomousDroid.h>
#include <SweepSensorHCSR04.h>
#include <AutonomousDroidSettings.h>
#include <SensorsSubSystem.h>
#include <EnginesSubSystemL298.h>
#include <NavigationSubSystem.h>
#include <RechargeSubSystem.h>
#include <ArduinoI2CSubSystem.h>
#include <ArduinoKeypadCommandSubSystem.h>
#include <I2CSubSystem.h>
#include <ArduinoI2CSubSystem.h>
#include <CommandInterpreter.h>

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

char inData[20]; // Allocate some space for the string
char inChar; // Where to store the character read
byte index = 0; // Index into array; where to store the character
bool isValidInput;
bool stopFlag = false;

void setup()
{
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
  isValidInput =false;
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

void loop() {
  
  Serial.println( "-----------------------------" );
  Serial.println( "Calibration of droid engines using arduino shield L298 V2");
  Serial.println( "-----------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "l# Rotate 90 left" );
  Serial.println( "r# Rotate 90 right" );
  Serial.println( "xx# Rotate xx degree");
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
    if (strcmp(inData,"l") == 0) {
      Serial.println("rotate 90 l");
      engines->go90Left();
      isValidInput = true;
    } else if (strcmp(inData,"r") == 0) {
      Serial.println("rotate 90 r");
      engines->go90Right();
      isValidInput = true;
    } else if (isValidNumber(inData, index - 1)){
      Serial.println("rotate degree");
      engines->turnNrDegree(atol(inData));
      isValidInput = true;
    } else {
      isValidInput = false;
    }
  } while( isValidInput == true );
}

