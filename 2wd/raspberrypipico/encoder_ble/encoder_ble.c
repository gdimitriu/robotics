/*
 * encoder_ble.c
 *
 *  Created on: July 11, 2022
 *      Author: Gabriel Dimitriu 2022
 */
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <pico/printf.h>
#include <string.h>
#include <stdlib.h>
#include "2enginesmove.h"
#include "configuration.h"

#define BLE_BUFFER 256
#define BLE_BUFFER_SEND 512


int bufferIndex = 0;
int absoluteMaxPower = 65025;
int maxPower = 65025;
int currentPower = 65025;
int minPower = 30000;
char inChar = '\0';
char bufferReceive[BLE_BUFFER];
char bufferSend[BLE_BUFFER_SEND];
bool isValidInput = false;

/*
 * engine settings
 */
float whellRadius = 3.25;
const float pi = 3.14f;
//left engine
unsigned int leftMotorPin1 = 15;
unsigned int leftMotorPin2 = 14;
unsigned int leftMotorEncoder = 16;
long leftResolutionCodor = 20;
float leftPPI;
//right engine
unsigned int rightMotorPin1 = 11;
unsigned int rightMotorPin2 = 10;
unsigned int rightMotorEncoder = 17;
long rightResolutionCodor = 20;
float rightPPI;
long countRotate90Left= 5;
long countRotate90Right= 5;

//sensors
unsigned int frontSensorPin = 13;
unsigned int backSensorPin = 12;
/*
 * compute the PPI from the new configuration
 */
void computePPI() {
	leftPPI = leftResolutionCodor/(2*pi*whellRadius);
	rightPPI = rightResolutionCodor/(2*pi*whellRadius);
}

/*
 * validate if the data is a number
 */
bool isValidNumber(char *data, int size) {
	if (size == 0 )
		return false;
	for (int i =0 ;i < size; i++) {
		if (!(data[i] < 58 && data[i] > 47 ))
			return false;
	}
	return true;
}

/*
 * clean the input buffers
 */
void makeCleanup() {
	for (bufferIndex = 0; bufferIndex < BLE_BUFFER; bufferIndex++) {
		bufferReceive[bufferIndex] = '\0';
	}
	bufferIndex = 0;
	inChar ='\0';
}

/*
 * make the move of the platform
 */
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
		 * return current power value
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
			sprintf(bufferSend,"left: %lu right: %lu\r\n",getLeftEngineCounts(),getRightEngineCounts());
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
			sprintf(bufferSend,"left: %lu right: %lu\r\n",getLeftEngineCounts(),getRightEngineCounts());
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
			if (atol(bufferReceive) > absoluteMaxPower || atol(bufferReceive) < 0) {
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
			setHumanCommand(true);
			setHumanDirection(moveData);
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
		 * move distance or rotate using degree or specific
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
//	gpio_init(leftMotorEncoder);
//    gpio_set_dir(leftMotorEncoder, GPIO_IN);
//    gpio_pull_down(leftMotorEncoder);
	gpio_set_irq_enabled_with_callback(leftMotorEncoder, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
//	gpio_init(rightMotorEncoder);
//    gpio_set_dir(rightMotorEncoder, GPIO_IN);
//    gpio_pull_down(rightMotorEncoder);
	gpio_set_irq_enabled_with_callback(rightMotorEncoder, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	gpio_set_irq_enabled_with_callback(frontSensorPin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
	gpio_set_irq_enabled_with_callback(backSensorPin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &gpio_callback);
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
