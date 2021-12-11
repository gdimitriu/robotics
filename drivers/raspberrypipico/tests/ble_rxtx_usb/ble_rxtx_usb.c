/*
 * ble_rxtx_usb.c
 *
 *  Created on: Dec 10, 2021
 *      Author: Gabriel Dimitriu 2021
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>

int main() {
	 char buffer[256];
	 int index = 0;
	 stdio_usb_init();
	 //initialize UART 1
	 uart_init(uart1, 38400);

	 // Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	 gpio_set_function(4, GPIO_FUNC_UART);
	 gpio_set_function(5, GPIO_FUNC_UART);
	 uart_set_translate_crlf(uart1,1);

	 while (1) {
		 if (uart_is_readable(uart1)) {
			 char readed =uart_getc(uart1);
			 buffer[index] = readed;
			 if (readed == '#') {
				 buffer[index+1] = '\0';
				 printf("%s\n",buffer);
				 index = 0;
			 } else {
				 index++;
			 }
		 }
	 }
}
