/*
  I2C slave controled.
  Copyright 2018 Gabriel Dimitriu

  This is part of Robotics
  
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
#include<Wire.h>

#define NODE_ADDRESS 1
/**
 * A4 to bus
 * A5 to bus
 * GND to GND Bus
 */
#define NODE_READ_DELAY 100
#define PHOTOREZISTOR_PIN A0
/**
 * Photoresistor conectivity
 * VCC to photresistor first pin
 * photoresistor second pin to A0
 * photoresistor second pin to R=10k first pin
 * R=10K second pin to GND
 */
#define BUFFER_SIZE 2
byte messageFromMaster[BUFFER_SIZE];
byte messageToMaster[255];

void setup() {
  pinMode(PHOTOREZISTOR_PIN, INPUT);
  Serial.begin(9600);  
  Serial.print("SLAVE #");
  Serial.println(NODE_ADDRESS);
  Wire.onRequest(requestHandler);
  //Wire.onReceive(receiveHandler);
  Wire.begin(NODE_ADDRESS);  // Activate I2C network
}

void loop() {
  // put your main code here, to run repeatedly:
  receiveHandler(2);
}


void receiveHandler(int nrOfBytes) {
  int pos = 0;
  while(Wire.available()) {
    messageFromMaster[pos] = Wire.read();
    pos++;
    if (pos == BUFFER_SIZE) {
      break;
    }
  }
  if(pos == 0) {
    return;
  }
  Serial.println((char *)messageFromMaster);
  if (messageFromMaster[0]==0x01 && messageFromMaster[1]=='#') {
    Wire.beginTransmission(0);
    String str = String(analogRead(PHOTOREZISTOR_PIN));
    Serial.print("Photoresistor value=");Serial.println(str);
    for(int i = 0 ; i<255;i++) {
      messageToMaster[i]='\0';
    }
    str.getBytes(messageToMaster, 255);
    for(int i = 0; i< 255;i++) {
      Wire.write(messageToMaster[i]);
    }
    Wire.endTransmission();
    Serial.println("Data sent to master");
  }
}

void requestHandler() {
  Wire.write('s');
}
