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
#include <AutonomousDroid.h>
#include <AutonomousDroidSettings.h>
#include <ArduinoKeypadCommandSubSystem.h>
#include <I2CSubSystem.h>
#include <ArduinoI2CSubSystem.h>
#include <CommandInterpreter.h>


#define COMMAND_BUTTON_PIN 19

#define MIN_DISTANCE 200
#define MAX_DISTANCE 4000
#define STOP_DISTANCE 100

#define LCD_ROWS 4
#define LCD_COLS 80

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

char colPins[KEYPAD_COLS] = {31,30,29,28}; //connect to the row pinouts of the keypad (31,30,29,28) vs orig {6,5,4,3}
char rowPins[KEYPAD_ROWS] = {35,34,33,32}; //connect to the column pinouts of the keypad (35,34,33,32) vs orig {11, 10, 9,8 }

AutonomousDroidSettings *settings;
ArduinoKeypadCommandSubSystem *commandSubSystem;
ArduinoKeypadSettings * keypadSettings;
I2CSubSystem *i2cBus;

void setup() {
  Serial.begin(9600);
  settings = new AutonomousDroidSettings();
  settings->setMaxDistance(MAX_DISTANCE);
  settings->setMinDistance(MIN_DISTANCE);
  settings->setTiming90Left(800);
  settings->setTiming90Right(800);
  settings->setTimingSlowForward(100);
 
  i2cBus = new ArduinoI2CSubSystem(0x01);
  i2cBus->addLCDDevice(LCD_ADDRESS, LCD_COLS, LCD_ROWS);
  i2cBus->initialize();
  keypadSettings = new ArduinoKeypadSettings(KEYPAD_ROWS,KEYPAD_COLS);
  keypadSettings->setKeyMap(makeKeymap(keys));
  keypadSettings->setRowPins(rowPins);
  keypadSettings->setColPins(colPins);
  
  keypadSettings->setCommandButton(COMMAND_BUTTON_PIN);
  commandSubSystem = new ArduinoKeypadCommandSubSystem(i2cBus, keypadSettings); 
  commandSubSystem->setCommandInterpreter(new CommandInterpreter());
  commandSubSystem->setAutonomousDroidSettings(settings);
  Serial.println("all initialize");
}

void loop() {
  if (commandSubSystem->isEnabledCommandISR()) {
    if (commandSubSystem->isReady()) {
      commandSubSystem->reset(commandSubSystem->getCommand());
    }
    Serial.println("has isr");
  }
}

