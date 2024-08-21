/*
 * IOE-SR05 using multicore and mutex and pio (uart_rx from pico-examples)
 * Copyright 2024 Gabriel Dimitriu
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

#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <hardware/uart.h>
#include <pico/printf.h>
#include <pico/mutex.h>
#include <pico/multicore.h>

#define SERIAL_BAUD 9600

// You'll need a wire from GPIO4 -> GPIO3
#define SR05EN  4
#define PIO_RX_PIN 5



static mutex_t distanceMutex;
static volatile uint16_t currentDistance = 2000;

void updateDistance() {
	uint8_t S_DATA = 0;
	uint8_t H_DATA = 0;
	uint8_t L_DATA = 0;
	uint8_t SUM = 0;

	uint16_t distance = 0;
	gpio_put(SR05EN, false);
	S_DATA = uart_getc(uart1);
	if ( S_DATA == 0xFF ) {
		H_DATA = uart_getc(uart1);
		L_DATA = uart_getc(uart1);
		SUM = uart_getc(uart1);
	}
	gpio_put(SR05EN, true);
	if ( S_DATA == 0xFF && H_DATA == 0xAA && L_DATA == 0xAA && SUM == 53 ) {
		distance = 2000;
	} else {		
		uint16_t sum = S_DATA + H_DATA + L_DATA;
		sum = sum << 8;
		sum = sum >> 8; 
		if ( SUM == sum ) {
			distance = H_DATA * 256  +  L_DATA;
		}
	}
	mutex_enter_blocking(&distanceMutex);
	currentDistance = distance;
	mutex_exit(&distanceMutex);
}

uint16_t getDistance() {
	uint16_t distance;
	mutex_enter_blocking(&distanceMutex);
	distance = currentDistance;
	mutex_exit(&distanceMutex);
	return distance;
}

void core1_entry() {
	while(1) {
		updateDistance();
	}
}

int main() {
    stdio_init_all();
	//initialize UART 1
	uart_init(uart1, SERIAL_BAUD);
	// Set the GPIO pin mux to the UART - 4 is TX, 5 is RX
	//gpio_set_function(4, GPIO_FUNC_UART);
	gpio_set_function(PIO_RX_PIN, GPIO_FUNC_UART);
	uart_set_translate_crlf(uart1, 1);
	uart_set_fifo_enabled(uart1, false);
	gpio_init(SR05EN);
    gpio_set_dir(SR05EN, GPIO_OUT);
	gpio_put(SR05EN, true);
    
	mutex_init(&distanceMutex);
	getchar();
	printf("Started ios-sr05 multicore on hw uart\n");
	fflush(stdout);
	multicore_reset_core1();
	multicore_launch_core1(core1_entry);
    while (true) {
		printf("getDistance\n");
		fflush(stdout);
        printf("CurrentDistance %u mm\n",getDistance());
		sleep_ms(100);
    }
}
