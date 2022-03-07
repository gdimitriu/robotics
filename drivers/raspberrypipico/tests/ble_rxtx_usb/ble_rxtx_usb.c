/*
 * ble_rxtx_usb.c
 *
 *  Created on: Dec 10, 2021
 *      Author: Gabriel Dimitriu 2021
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <string.h>

int main() {
	char buffer[256];
	char bufferSend[512];
	int index = 0;

	stdio_usb_init();
	//initialize UART 1
	uart_init(uart1, 38400);

	// Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	gpio_set_function(4, GPIO_FUNC_UART);
	gpio_set_function(5, GPIO_FUNC_UART);
	uart_set_translate_crlf(uart1, 1);

	while (1) {
		if (uart_is_readable(uart1)) {
			char readed = uart_getc(uart1);
			buffer[index] = readed;
			if (readed == '#') {
				buffer[index + 1] = '\0';
				printf("%s\n", buffer);
				index = 0;
				if (uart_is_writable(uart1)) {
					memset(bufferSend,'\0',sizeof(char)*512);
					sprintf(bufferSend,"Received=%s\n",buffer);
					uart_puts(uart1,bufferSend);
				}
				if (strcmp(buffer, "exit#") == 0) {
					return 1 ;
				}
			} else {
				index++;
			}
		}
	}
}
