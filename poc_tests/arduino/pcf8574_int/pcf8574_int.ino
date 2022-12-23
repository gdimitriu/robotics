#include <Wire.h>
#include<PCF8574.h>
int Pin0 = 0;
int Pin1 = 1;
int Pin2 = 2;
#define PIN0_MASK 0b00000001
#define PIN1_MASK 0b00000010
#define PIN2_MASK 0b00000100
PCF8574 expander;
volatile bool detectedInt = false;
void detectInterrupt(void) {
  detectedInt = true;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  expander.begin(0x21);
  expander.pinMode(Pin0, INPUT_PULLUP);
  expander.pinMode(Pin1, INPUT_PULLUP);
  expander.pinMode(Pin2, INPUT_PULLUP);
  expander.enableInterrupt(2,detectInterrupt);
}
uint8_t sensors = 0;
void loop() {
  /*
  sensors = expander.read();
  uint8_t pin = sensors << 7;
  pin = pin >> 7;
  if (pin == 1) {
    Serial.println("Pin0 off");
  }
  pin = sensors << 6;
  pin = pin >> 7;
  if (pin == 1) {
    Serial.println("Pin1 off");
  }
  pin = sensors << 5;
  pin = pin >> 7;
  if (pin == 1) {
    Serial.println("Pin2 off");
  } */
  while(detectedInt == false)
  // loop until detect
  ;
  sensors = expander.read();
  Serial.println(sensors);
  if (( sensors & PIN0_MASK) > 0) {
    Serial.println("Pin0 off");
  } else {
    Serial.println("Pin0 on");
  }
  if (( sensors & PIN1_MASK) > 0) {
    Serial.println("Pin1 off");
  } else {
    Serial.println("Pin1 on");
  }
  if (( sensors & PIN2_MASK) > 0) {
    Serial.println("Pin2 off");
  } else {
    Serial.println("Pin2 on");
  }
  detectedInt = false;
}
