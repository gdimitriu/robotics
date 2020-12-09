/**
 * Copyed from Introducere in Arduino by Optimus Digital
 */
/* COMMANDS:
 *  AT -> OK
 *  AT+UART=38400,0,0
 *  AT+NAME=<NAME>
 */
#include <SoftwareSerial.h>

#define RxD 2 //se conecteaza la pinul digital 5
#define TxD 3 //se conecteaza la pinul digital 4

SoftwareSerial BTSerial(RxD, TxD);
void setup() {
  BTSerial.begin(38400);
  Serial.begin(9600);
  Serial.println("Starting ...");
}
int i = 0;
void loop() {
  if (BTSerial.available())
    Serial.write(BTSerial.read());
  if (Serial.available()) 
    BTSerial.write(Serial.read());
}
