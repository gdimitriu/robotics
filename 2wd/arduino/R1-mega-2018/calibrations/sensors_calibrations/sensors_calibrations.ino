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

#define LEFT_SENSOR_PIN 7
#define CENTRAL_SENSOR_PIN 4
#define RIGHT_SENSOR_PIN 8

#define ECHO_PIN 10
#define TRIG_PIN 11
#define SENSOR_SERVO_PIN 12

#define SENSOR_BACKWARD_PIN 2

#define MIN_DISTANCE 200
#define MAX_DISTANCE 4000
#define STOP_DISTANCE 100
#define DEBUG_MODE true

AutonomousDroid *droid;
SweepSensorHCSR04 *frontSweepSensor;
AutonomousDroidSettings *settings;
SensorsSubSystem *sensors;
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
  droid->setSettings(settings);
  //sensors
  sensors = new SensorsSubSystem();
  sensors->attachFrontLeftSensor(LEFT_SENSOR_PIN, LOW);
  sensors->attachFrontCentralSensor(CENTRAL_SENSOR_PIN, LOW);
  sensors->attachFrontRightSensor(RIGHT_SENSOR_PIN, LOW);
  frontSweepSensor = new SweepSensorHCSR04(TRIG_PIN, ECHO_PIN);
  sensors->attachFrontSweepSensor(SENSOR_SERVO_PIN, frontSweepSensor);
  sensors->attachReverseSensor(SENSOR_BACKWARD_PIN, LOW);
  droid->setSensorsSubSystem(sensors);
  droid->setup();
  Serial.println("all initialized");
}

void loop() {
  bool left = false;
  bool center = false;
  bool right = false;
  unsigned long leftDistance = 0;
  unsigned long rightDistance = 0;
  Serial.println( "-----------------------------" );
  Serial.println( "Calibration of sensors");
  Serial.println( "-----------------------------" );
  Serial.println( "MENU:" );
  Serial.println( "f) Front sensors");
  Serial.println( "r) Reverse sensor");
  Serial.println( "1) Front left");
  Serial.println( "2) Front central");
  Serial.println( "3) Front right");
  Serial.println( "4) Look left");
  Serial.println( "5) Look front");
  Serial.println( "6) Look right");
  Serial.println( "7) Look left and right");
  Serial.println( "-----------------------------" );
  do
  {
    byte c;
    // get the next character from the serial port
    Serial.print( "?" );
    while( !Serial.available() )
      ; // LOOP...
    c = Serial.read();
    // execute the menu option based on the character recieved
    switch( c )
    {
      case 'f':
        left = false;
        center = false;
        right = false;
        sensors->readFrontSensors(left, center, right);
        Serial.print("front left="); Serial.println(left);
        Serial.print("front central="); Serial.println(center);
        Serial.print("front right="); Serial.println(right);
        stopFlag = true;
        break;
      case 'r':
        Serial.print("reverse="); Serial.println(sensors->isReverseObstructed());
        stopFlag = true;
        break;
      case '1':
        Serial.print("front left="); Serial.println(sensors->isFrontLeftObstructed());
        stopFlag = true;
        break;
      case '2':
        Serial.print("front cental="); Serial.println(sensors->isFrontCentralObstructed());
        stopFlag = true;
        break;
      case '3':
        Serial.print("front right="); Serial.println(sensors->isFrontRightObstructed());
        stopFlag = true;
        break;
      case '4':
        Serial.print("look left="); Serial.println(sensors->look(180));
        stopFlag = true;
        break;
      case '5':
        Serial.print("look front="); Serial.println(sensors->look(90));
        stopFlag = true;
        break;
      case '6':
        Serial.print("look right="); Serial.println(sensors->look(0));
        stopFlag = true;
        break;
      case '7':
        leftDistance = 0;
        rightDistance = 0;
        sensors->getLeftRightDistances(leftDistance, rightDistance);
        Serial.print("left distance="); Serial.println(leftDistance);
        Serial.print("right distance="); Serial.println(rightDistance);
        stopFlag = true;
        break;
      default:
        // wrong character! display the menu again!
        stopFlag = false;
        break;
    }
    } while( stopFlag == true );
}


