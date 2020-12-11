/*
Stepper move using Stepper lib.
Copyright 2019 Gabriel Dimitriu

This file is part of Robotics.

Robotics is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
Robotics is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with Robotics; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
*/
#include <Stepper.h>

#define STEPPER_PIN_1 8
#define STEPPER_PIN_2 9
#define STEPPER_PIN_3 10
#define STEPPER_PIN_4 11
#define STEPS 64

Stepper myStepper(STEPS,STEPPER_PIN_1,STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4); 
int pas = 0;
void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(STEPS);
}

void loop() {
  myStepper.step(64);
  pas++;
  if (pas>64) {
    pas = 0;
  }
  delay(10);
}
