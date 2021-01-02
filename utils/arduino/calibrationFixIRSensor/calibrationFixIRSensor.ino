/*
 * Calibration of a Fix IR Sensor.
 * Copyright 2020 Gabriel Dimitriu
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
#define SENSOR_PIN 7
#define SENSOR_ENABLE_PIN 13
#define LED_PIN 2

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_ENABLE_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  
  if (digitalRead(SENSOR_PIN) == LOW) {
     digitalWrite(LED_PIN, HIGH);
     Serial.println("DETECTED");
     delay(1000);
  } else {
    Serial.println("None");
    digitalWrite(LED_PIN, LOW);
  }
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
