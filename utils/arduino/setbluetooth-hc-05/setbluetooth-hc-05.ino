/**
 * Copyed from Introducere in Arduino by Optimus Digital
 */
/* for HC-05 is CRLF for HC-06 is no_line_ending
 *  COMMANDS:
 *  AT -> OK
 *  HC-05
 *  AT+UART=38400,0,0
 *  HC-06
 *  AT+BAUD6
 *  HC-05
 *  AT+NAME=<NAME>
 *  HC-06
 *  AT+NAME<NAME>
 *  COMMANDS for ESP01
 *  AT+CWMODE=3
 *  AT+CIPMUX=1
 *  AT+CIPSERVER=1,port
 *  AT+CIFSR
 */
#include <SoftwareSerial.h>
//#include <NeoSWSerial.h>
//#include <EnableInterrupt.h>

#define RxD 2 //se conecteaza la pinul digital 5
#define TxD 3 //se conecteaza la pinul digital 4

SoftwareSerial BTSerial(RxD, TxD);
//NeoSWSerial BTSerial(RxD, TxD);
//=================================================================
//====                    NEO SERIAL INTERRUPT                  ===
//=================================================================
/*void neoSSerial1ISR()
{
    NeoSWSerial::rxISR(*portInputRegister(digitalPinToPort(RxD)));
} */
void setup() {
  Serial.begin(9600);//115200);
  BTSerial.begin(9600);//default for HC-06
  //BTSerial.begin(38400);  
//  enableInterrupt(RxD, neoSSerial1ISR, CHANGE);
  Serial.println("Starting ...");
}
int i = 0;
void loop() {
  if (BTSerial.available()) {
    Serial.write(BTSerial.read());
  }
  if (Serial.available()) 
    BTSerial.write(Serial.read());
}
