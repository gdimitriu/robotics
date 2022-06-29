/*
 * 2enginesbleenc.c
 *
 *  Created on: June 27, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <string.h>
#include <stdbool.h>

#define SERIAL_DEBUG_MODE true
#define BLE_BUFFER 256
#define BLE_BUFFER_SEND 512
#define ABSOLUTE_MAX_POWER 65025
#define HIGH 1
#define LOW 0
int bufferIndex = 0;
int maxPower = ABSOLUTE_MAX_POWER;
int currentPower = ABSOLUTE_MAX_POWER;
int minPower = 30000;
char inChar = '\0';
char bufferReceive[BLE_BUFFER];
char bufferSend[BLE_BUFFER_SEND];
bool isValidInput = false;

/*
 * engine settings
 */
float whellRadius = 2;
const float pi = 3.14f;
//left engine
unsigned int leftMotorPin1 = 15;
unsigned int leftMotorPin2 = 14;
unsigned int leftMotorEncoder = 6;
volatile unsigned long leftCounts;
long leftResolutionCodor = 1450;
float leftPPI;
float leftCurrentDistance = 0.0f; 
//right engine
unsigned int rightMotorPin1 = 11;
unsigned int rightMotorPin2 = 10;
unsigned int rightMotorEncoder = 7;
volatile unsigned long rightCounts;
long rightResolutionCodor = 1450;
float rightPPI;
float rightCurrentDistance = 0.0f;
long countRotate90Left= 841;
long countRotate90Right= 830;
long countRotate1Inner = 9;
long countRotate1Outer = 10;

void computePPI() {
	leftPPI = leftResolutionCodor/(2*pi*whellRadius);
	rightPPI = rightResolutionCodor/(2*pi*whellRadius);
}

bool isValidNumber(char *data, int size) {
	if (size == 0 )
		return false;
	for (int i =0 ;i < size; i++) {
		if (!(data[i] < 58 && data[i] > 47 ))
			return false;
	}
	return true;
}

void gpio_callback_encoder(uint gpio, uint32_t events) {
    if (GPIO_IRQ_EDGE_RISE == events) {
    	if (gpio == leftMotorEncoder) {
			leftCounts++;
		} else if (gpio == rightMotorEncoder) {
			rightCounts++;
		}
    }
}

void makeCleanup() {
  for (bufferIndex = 0; bufferIndex < BLE_BUFFER; bufferIndex++)
  {
    bufferReceive[bufferIndex] = '\0';
  }
  bufferIndex = 0;
  inChar ='\0';
}

void clearEncoders() {
	leftCounts = 0;
	rightCounts = 0;
}

void breakEngines() {
	pwm_set_gpio_level(leftMotorPin1,ABSOLUTE_MAX_POWER);
    pwm_set_gpio_level(leftMotorPin2,ABSOLUTE_MAX_POWER);
    pwm_set_gpio_level(rightMotorPin1,ABSOLUTE_MAX_POWER);
    pwm_set_gpio_level(rightMotorPin2,ABSOLUTE_MAX_POWER);
}

/*
* Move the platform in predefined directions.
*/
void go(int speedLeft, int speedRight) {
	if (speedLeft == 0 && speedRight == 0 ) {
		pwm_set_gpio_level(leftMotorPin1,LOW);
		pwm_set_gpio_level(leftMotorPin2,LOW);
		pwm_set_gpio_level(rightMotorPin1,LOW);
		pwm_set_gpio_level(rightMotorPin2,LOW);
#ifdef SERIAL_DEBUG_MODE    
		printf("All on zero\n");
#endif
		return;
	}
	if (speedLeft > 0) {
		pwm_set_gpio_level(leftMotorPin1, speedLeft);
		pwm_set_gpio_level(leftMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Left %d,0\n",speedLeft);
#endif
	} 
	else {
		pwm_set_gpio_level(leftMotorPin1, 0);
		pwm_set_gpio_level(leftMotorPin2, -speedLeft);
#ifdef SERIAL_DEBUG_MODE
		printf("Left 0,%d\n",-speedLeft);
#endif
	}
 
	if (speedRight > 0) {
		pwm_set_gpio_level(rightMotorPin1, speedRight);
		pwm_set_gpio_level(rightMotorPin2, 0);
#ifdef SERIAL_DEBUG_MODE
		printf("Right %d,0\n",speedRight);
#endif
	}else {
		pwm_set_gpio_level(rightMotorPin2, 0);		
		pwm_set_gpio_level(rightMotorPin2, -speedRight);
#ifdef SERIAL_DEBUG_MODE
		printf("Right 0,%d\n",-speedRight);
#endif
	}
}

void stopLeftEngine() {
	pwm_set_gpio_level(leftMotorPin1,LOW);
	pwm_set_gpio_level(leftMotorPin2,LOW);
#ifdef SERIAL_DEBUG_MODE    
	printf("Stop left engine\n");
#endif
}

void stopRightEngine() {
	pwm_set_gpio_level(rightMotorPin1,LOW);
	pwm_set_gpio_level(rightMotorPin2,LOW);
#ifdef SERIAL_DEBUG_MODE    
	printf("Stop right engine\n");
#endif
}

void moveWithDistance(float moveData) {
	
	bool stopLeft = false;
	bool stopRight = false;
	float distance;
	if (moveData > 0) {
		distance = moveData;
		go(currentPower,currentPower);
	} else if (moveData < 0) {
		distance = - moveData;
		go(-currentPower,-currentPower);
	} else {
		go(0,0);
		return;
	}
	while( !stopLeft || !stopRight){
		if (!stopLeft) {
			leftCurrentDistance = leftCounts / leftPPI;
			if((distance - leftCurrentDistance) <= 0.2) {
				stopLeftEngine();
				stopLeft = true;
			}
		}
		if (!stopRight) {
			rightCurrentDistance = rightCounts / rightPPI;
			if ((distance - rightCurrentDistance) <= 0.2) {
				stopRightEngine();
				stopRight=true;
			}
		}
	}
	go(0,0);
	leftCurrentDistance = 0;
	rightCurrentDistance = 0;
}

void rotate90Left() {
  go(-currentPower,currentPower);
  while(leftCounts < countRotate90Left && rightCounts < countRotate90Right);
  go(0,0);
}

void rotate90Right() {
  go(currentPower,-currentPower);
  while(leftCounts < countRotate90Left && rightCounts < countRotate90Right);
  go(0,0);
}

void rotateLeftDegree(int nr) {
  go(-currentPower,currentPower);
  while(leftCounts < countRotate1Inner*nr && rightCounts < countRotate1Outer*nr);
  go(0,0);
}

void rotateRightDegree(int nr) {
  go(currentPower,-currentPower);
  while(leftCounts < countRotate1Outer*nr && rightCounts < countRotate1Inner*nr);
  go(0,0);
}

/*
 * Move the platform with dinstance or rotate with encoders
 */
void moveOrRotateWithDistance(float moveData, int rotateData) {
	go(0,0);
	clearEncoders();
	if (rotateData == 0) {
#ifdef SERIAL_DEBUG_MODE
	printf("Move linear to %f\r\n", moveData);
#endif
		moveWithDistance(moveData);
	} else if (fabs(moveData) <= 0.01) {
		if (rotateData == -1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees left\r\n");
#endif
			rotate90Left();
		} else if (rotateData == 1) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate 90 degrees right\r\n");
#endif
			rotate90Right();
		} else if (rotateData < 0) {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees left\r\n",rotateData);
#endif
			rotateLeftDegree(rotateData);
		} else {
#ifdef SERIAL_DEBUG_MODE
			printf("Rotate %d degrees right\r\n",rotateData);
#endif
			rotateRightDegree(rotateData);
		}
	}
}

bool makeMove() {
  if (bufferIndex > 0) {
     bufferReceive[bufferIndex] = '\0';
  }
  if (strlen(bufferReceive) == 1) {
	if (bufferReceive[0] == 'I')  {
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"unsupported\r\n");
		uart_puts(uart1,bufferSend);		
	}
	/*
	 * return maximum power setting
	 */
	else if (bufferReceive[0] == 'V') {      
#ifdef SERIAL_DEBUG_MODE
		printf("%d\n\r",maxPower);
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",maxPower);
		uart_puts(uart1,bufferSend);
	}
	/*
	 * return minimum power setting
	 */ 
    else if (bufferReceive[0] =='v') {
#ifdef SERIAL_DEBUG_MODE
      printf("%d\n",minPower);
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",minPower);
		uart_puts(uart1,bufferSend);
    }
    /*
	 * return current pwower value
	 */
    else if (bufferReceive[0] =='c') {
#ifdef SERIAL_DEBUG_MODE
      printf("%d\n",currentPower);
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",currentPower);
		uart_puts(uart1,bufferSend);
    }
    /*
	 * return minimum low power distance
	 */
    else if (bufferReceive[0] == 'd') {
#ifdef SERIAL_DEBUG_MODE
		printf("unsupported\n");
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",0);
		uart_puts(uart1,bufferSend);
    }
    /*
	 * return stop distance
	 */
    else if (bufferReceive[0] == 's') {
#ifdef SERIAL_DEBUG_MODE
		printf("unsupported\n");
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",0);
		uart_puts(uart1,bufferSend);
    }
    /*
	 * break the engines
	 */
    else if (bufferReceive[0] == 'b') {
		breakEngines();
		isValidInput = false;
		makeCleanup();
		return false;
    }
    /*
	 * return encoder values
	 */
    else if (bufferReceive[0] == 'C') {		
        //print the encoders leftCounts
        memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"left: %lu right: %lu\r\n",leftCounts,rightCounts);
		uart_puts(uart1,bufferSend);
        makeCleanup();
        isValidInput = true;
        return true;
	  }
	  /*
	   * reset encoder values
	   */
	  else if (bufferReceive[0] == 'R') {
        //print the encoders leftCounts
        memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"left: %lu right: %lu\r\n",leftCounts,rightCounts);
		uart_puts(uart1,bufferSend);
		//reset counters
		clearEncoders();
        makeCleanup();
        isValidInput = true;
        return true;
	  }
	  /*
	   * unkown command
	   */
	  else {
#ifdef SERIAL_DEBUG_MODE
		printf("unsupported\n");
#endif
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"%d\r\n",0);
		uart_puts(uart1,bufferSend);
		isValidInput = false;
    }
  }
	/*
	* Command with data
	*/
  else {
	/*
	 * set the maximum power value
	 */
	if (bufferReceive[0] == 'V') {
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
		sprintf(bufferSend,"OK\r\n");
		uart_puts(uart1,bufferSend);
		//remove V from command
		for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
			bufferReceive[i]=bufferReceive[i+1];
        }
        if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(bufferReceive) > ABSOLUTE_MAX_POWER || atol(bufferReceive) < 0) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef SERIAL_DEBUG_MODE
        printf("MaxPower=%s\n",bufferReceive);
#endif        
        maxPower = atol(bufferReceive);
        makeCleanup();
        isValidInput = true;
        return true;
	}
	/*
	 * set the minimum power
	 */
	else if (bufferReceive[0] == 'v') {
		memset(bufferSend,'\0',sizeof(char)*BLE_BUFFER_SEND);
        sprintf(bufferSend,"OK\r\n");
		uart_puts(uart1,bufferSend);
        //remove v from command
        for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
          bufferReceive[i]=bufferReceive[i+1];
        }
        if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(bufferReceive) > maxPower || atol(bufferReceive) < 0) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef SERIAL_DEBUG_MODE
        printf("MinPower=%s\n",bufferReceive);
#endif                
        minPower = atol(bufferReceive);
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * set current power value
	   */
      else if (bufferReceive[0] == 'c') {
        sprintf(bufferSend,"OK\r\n");
        uart_puts(uart1,bufferSend);
        //remove c from command
        for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
          bufferReceive[i]=bufferReceive[i+1];
        }
        if (!isValidNumber(bufferReceive, bufferIndex - 2)) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
        if (atol(bufferReceive) > maxPower || atol(bufferReceive) < 0) {
          isValidInput = false;
          makeCleanup();
          return false;
        }
#ifdef SERIAL_DEBUG_MODE
        printf("CurrentPower=%s\n",bufferReceive);
#endif                
        currentPower = atol(bufferReceive);
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * set the low power distance value
	   */
      else if (bufferReceive[0] == 'd') {
        sprintf(bufferSend,"OK\r\n");
        uart_puts(uart1,bufferSend);
        //this does not support low power distance
#ifdef SERIAL_DEBUG_MODE
		printf("unsupported\n");
#endif        
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * set the stop distance value
	   */
      else if (bufferReceive[0] == 's') {
        sprintf(bufferSend,"OK\r\n");
		uart_puts(uart1,bufferSend);
        //this does not support stop distance
#ifdef SERIAL_DEBUG_MODE
		printf("unsupported\n");
#endif        
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * move or rotate until break command
	   */
      else if (bufferReceive[0] == 'M') {
        //remove M from command
        for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
          bufferReceive[i]=bufferReceive[i+1];
        }
        bufferReceive[strlen(bufferReceive)] = '\0';
        int position;
        for (uint8_t i = 0; i < strlen(bufferReceive); i++) {
          if (bufferReceive[i] == ',') {
            position = i;
            break;
          }
        }
        char buf[10];
        for (int i = 0; i < 10; i++) {
          buf[i] = '\0';
        }
        for (int i = 0 ; i < position; i++) {
          buf[i] = bufferReceive[i];
        }
        int moveData = atoi(buf);
        for (int i = 0; i < 10; i++) {
          buf[i] = '\0';
        }
        int idx = 0;
        for (int i = position + 1; i < strlen(bufferReceive); i++) {
          buf[idx] = bufferReceive[i];
          idx++;
        }
        int rotateData = atoi(buf);
#ifdef SERIAL_DEBUG_MODE
		printf("%s\n%d:%d\n",bufferReceive,moveData,rotateData);
#endif        
        if (moveData == 0 && rotateData == 0) {
          go(0,0);
        } else if (rotateData == 0) {
          if (moveData < 0) {
            go(-currentPower,-currentPower);
          } else {
            go(currentPower, currentPower);
          }
        } else {
          if (rotateData < 0) {
            go(-currentPower,currentPower);
          } else {
            go(currentPower, -currentPower);
          }
        }
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * move distance or rotate using nr encorder number or specific
	   */
      else if (bufferReceive[0] == 'm') {
        //remove m from command
        for (uint8_t i = 0 ; i < strlen(bufferReceive); i++) {
          bufferReceive[i]=bufferReceive[i+1];
        }
        bufferReceive[strlen(bufferReceive)] = '\0';
        int position;
        for (uint8_t i = 0; i < strlen(bufferReceive); i++) {
          if (bufferReceive[i] == ',') {
            position = i;
            break;
          }
        }
        char buf[10];
        for (int i = 0; i < 10; i++) {
          buf[i] = '\0';
        }
        for (int i = 0 ; i < position; i++) {
          buf[i] = bufferReceive[i];
        }
        float moveData = atof(buf);
        for (int i = 0; i < 10; i++) {
          buf[i] = '\0';
        }
        int idx = 0;
        for (int i = position + 1; i < strlen(bufferReceive); i++) {
          buf[idx] = bufferReceive[i];
          idx++;
        }
        int rotateData = atoi(buf);
#ifdef SERIAL_DEBUG_MODE
		printf("%s\n%f:%d\n",bufferReceive,moveData,rotateData);
#endif        
        if (moveData == 0 && rotateData == 0) {
          go(0,0);
        } else {
			moveOrRotateWithDistance(moveData,rotateData);
        }
        makeCleanup();
        isValidInput = true;
        return true;
      }
      /*
	   * command unkown
	   */
      else {
        sprintf(bufferSend,"%d\r\n",0);
		uart_puts(uart1,bufferSend);
        makeCleanup();
        isValidInput = false;
        return false;
      }
    }
    makeCleanup();
    return true;
}

int main() {
	stdio_usb_init();
	//initialize UART 1
	uart_init(uart1, 38400);
	// Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	gpio_set_function(4, GPIO_FUNC_UART);
	gpio_set_function(5, GPIO_FUNC_UART);
	uart_set_translate_crlf(uart1, 1);
#ifdef SERIAL_DEBUG_MODE
	printf("Starting...\n");
	fflush(stdout);
#endif
	gpio_set_function(leftMotorPin1, GPIO_FUNC_PWM);
	// Figure out which slice we just connected
    uint slice_num = pwm_gpio_to_slice_num(leftMotorPin1);
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
	gpio_set_function(leftMotorPin2, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(leftMotorPin2);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);	
	gpio_set_function(rightMotorPin1, GPIO_FUNC_PWM);	
	slice_num = pwm_gpio_to_slice_num(rightMotorPin1);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);	
	gpio_set_function(rightMotorPin2, GPIO_FUNC_PWM);
	slice_num = pwm_gpio_to_slice_num(rightMotorPin2);
	config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);
	gpio_init(leftMotorEncoder);
    gpio_set_dir(leftMotorEncoder, GPIO_IN);
    gpio_pull_down(leftMotorEncoder);
	gpio_set_irq_enabled_with_callback(leftMotorEncoder, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback_encoder);
	gpio_init(rightMotorEncoder);
    gpio_set_dir(rightMotorEncoder, GPIO_IN);
    gpio_pull_down(rightMotorEncoder);
	gpio_set_irq_enabled_with_callback(rightMotorEncoder, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback_encoder);
	//read from external EEPROM configuration
	//update configuration
	computePPI();
	
	makeCleanup();
#ifdef SERIAL_DEBUG_MODE	
	printf("Started\n");
	fflush(stdout);
#endif	
	while (1) {
		if (uart_is_readable(uart1)) {
			inChar = uart_getc(uart1);
			//commands does not have terminators
			if(inChar == '\n' || inChar == '\r' || inChar == ' ' || inChar == '\t' || inChar == '\0' || inChar < 35 || inChar > 122) {
				continue;
			}
			//commands start with a letter capital or small
			if (bufferIndex == 0 && !((inChar >64 && inChar <91) || (inChar > 96 && inChar<123))) {
				continue;
			}
			bufferReceive[bufferIndex] = inChar;
			if (inChar == '#') {
				bufferReceive[bufferIndex + 1] = '\0';
				if (strcmp(bufferReceive, "exit#") == 0) {
					return 1 ;
				}
#ifdef SERIAL_DEBUG_MODE				
				printf("<<%s>>index=%d\n", bufferReceive,bufferIndex);
#endif			
				makeMove();
				makeCleanup();
			} else {
				bufferIndex++;
			}
		}
	}
}
