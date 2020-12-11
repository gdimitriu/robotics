/*
Stepper move calibration using PCF8574 as extender for Arduino.
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
#include <Wire.h>
#include<PCF8574.h>
int Pin0 = 0;
int Pin1 = 1;
int Pin2 = 2;
int Pin3 = 3;
int pas = 0;
boolean directie = false;
PCF8574 expander;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();
  expander.begin(0x20);
  expander.pinMode(0, OUTPUT);
  expander.pinMode(1, OUTPUT);
  expander.pinMode(2, OUTPUT);
  expander.pinMode(3, OUTPUT);
}

void loop()
{
  switch (pas%7) {
    case 0:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, HIGH);
      break;
    case 1:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, HIGH);
      break;
    case 2:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 3:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, HIGH);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 4:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 5:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, HIGH);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 6:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
    case 7:
      expander.digitalWrite(Pin0, HIGH);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, HIGH);
      break;
    default:
      expander.digitalWrite(Pin0, LOW);
      expander.digitalWrite(Pin1, LOW);
      expander.digitalWrite(Pin2, LOW);
      expander.digitalWrite(Pin3, LOW);
      break;
  }
  Serial.print("pas=");Serial.println(pas);
  pas++;
  delay(1);
}
