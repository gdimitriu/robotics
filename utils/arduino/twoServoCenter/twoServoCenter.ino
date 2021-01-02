/*
 * 2 Servo put at center this is useful to put servo at center when the sensor is attached
 * Copyright 2020 Gabriel Dimitriu
 *
 * This is part of Robotics project.
  
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
#include <Servo.h>

#define SERVO1_PIN 4
#define SERVO2_PIN 7
Servo firstServo;
Servo secondServo;
void setup() {
  firstServo.attach(SERVO1_PIN);
  secondServo.attach(SERVO2_PIN);
  firstServo.write(90);
  secondServo.write(90);
}

void loop() {
  firstServo.write(90);
  secondServo.write(90);
  delay(1000);
}
