/*
 * hc-sr04_lcd_multicore.cpp
 *
 *  Created on: Jan 15, 2023
 *      Author: Gabriel Dimitriu 2023
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>
#include <string.h>
#include "FreeMono12pt7b.h"
#include "ili934x.h"

#define ECHO_PIN 10
#define TRIG_PIN 11

#define PIN_SCK  2
#define PIN_MOSI 3
#define PIN_MISO 4
//not used
#define PIN_CS   22

#define PIN_DC   6
#define PIN_RST  5

#define SPI_PORT spi0
//do not use 8 and 9 is for uart1

ILI934X *display = NULL;

#define PICO_TIME_DEFAULT_ALARM_POOL_DISABLED 0

queue_t call_queue;
queue_t results_queue;

typedef struct
{
    uint32_t (*func)();
} queue_entry_t;

void core1_entry() {
	while(1) {
		queue_entry_t localEntry;
		printf("Starting core1\n");
		fflush(stdout);
        queue_remove_blocking(&call_queue, &localEntry);
		uint32_t (*func)() = (uint32_t(*)())localEntry.func;
		uint32_t result = (*func)();
		printf("result = %d\n",result);
		fflush(stdout);
        queue_add_blocking(&results_queue, &result);
		printf("waiting %u  %u\n", queue_get_level(&call_queue), queue_get_level(&results_queue));
		fflush(stdout);
	}
}

uint32_t getDistance() {
	printf("GetDistance\n");
	fflush(stdout);
	bool timeoutOccured = false;
	uint32_t currentDistance = 4500;
	gpio_put(TRIG_PIN, false);
	sleep_us(2);
	gpio_put(TRIG_PIN, true);
	sleep_us(10);
	gpio_put(TRIG_PIN, false);
	while(gpio_get(ECHO_PIN) == false);
	absolute_time_t start = get_absolute_time();
	while(gpio_get(ECHO_PIN) == true) {
		if (absolute_time_diff_us(start, get_absolute_time()) > 26190) {
			timeoutOccured = true;
			break;
		}
	}
	if (timeoutOccured == false) {
		absolute_time_t finish = get_absolute_time();
		int64_t microseconds = absolute_time_diff_us(start, finish);
		currentDistance = ((microseconds/2)/29.1) * 10; //cm * 10 to be mm
	}	
	return currentDistance;
}
int main() {
	char buffer[256];
	char lcdBuffer[256];

	stdio_init_all();
		
	//initialize lcd
	spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);

    display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, 240, 320, R90DEG);
    display->reset();
    display->init();
	display->clear();
	
	GFXfont *font = const_cast<GFXfont *>(&FreeMono12pt7b);
	int16_t x;
	int16_t y;
	int16_t minx;
	int16_t miny;
	int16_t maxx;
	int16_t maxy;
	char ch;
	char *str;
	printf("Starting\n");
	fflush(stdout);
	scanf("%s", buffer);
	fflush(stdin);
	printf("Receiving start command=%s\n", buffer);
	fflush(stdout);
	multicore_reset_core1();
	queue_init(&call_queue, sizeof(queue_entry_t), 2);
    queue_init(&results_queue, sizeof(uint32_t), 2);	
	multicore_launch_core1(core1_entry);
	printf("Starting HC-SR04_LCD\n");
	fflush(stdout);
	printf("Enter d# to get the value from HC-SR04\n");
	fflush(stdout);
	memset(lcdBuffer, 0, sizeof(lcdBuffer));
	uint32_t currentDistance;
	queue_entry_t entry;
			entry.func = &getDistance;
			queue_add_blocking(&call_queue, &entry);

			// We could now do a load of stuff on core 0 and get our result later
			printf("waiting %u  %u\n", queue_get_level(&call_queue), queue_get_level(&results_queue));
			fflush(stdout);
			queue_remove_blocking(&results_queue, &currentDistance);
						entry.func = &getDistance;
			queue_add_blocking(&call_queue, &entry);

			// We could now do a load of stuff on core 0 and get our result later
			printf("waiting %u  %u\n", queue_get_level(&call_queue), queue_get_level(&results_queue));
			fflush(stdout);
			queue_remove_blocking(&results_queue, &currentDistance);

	while (1) {
		
		printf("Input Command\n");
		fflush(stdout);
		scanf("%s",buffer);
		fflush(stdin);			
		printf("received: <<%s>>\n",buffer);
		fflush(stdout);
		sprintf(lcdBuffer,"received: <<%s>>",buffer);
		str = lcdBuffer;
		x = 10;
		y = 100;
		minx = 0;
		miny = 0;
		maxx = 0;
		maxy = 0;
		display->clear();
		while ((ch = *str++)) {
			// charBounds() modifies x/y to advance for each character,
			// and min/max x/y are updated to incrementally build bounding rect.
			display->charBounds(ch, &x, &y, &minx, &miny, &maxx, &maxy, font);
			display->drawChar(x,y,ch,display->colour565(255,0,0), font);
		}
		if (buffer[0] == 'd' && buffer[1] == '#') {
			uint32_t currentDistance;
			memset(lcdBuffer, 0, sizeof(lcdBuffer));
			queue_entry_t entry;
			entry.func = &getDistance;
			queue_add_blocking(&call_queue, &entry);

			// We could now do a load of stuff on core 0 and get our result later
			printf("waiting %u  %u\n", queue_get_level(&call_queue), queue_get_level(&results_queue));
			fflush(stdout);
			queue_remove_blocking(&results_queue, &currentDistance);
			sprintf(lcdBuffer, "Distance = %d mm", currentDistance);
			printf("%s\n",lcdBuffer);
			fflush(stdout);
			str = lcdBuffer;
			x = 10;
			y = 100 + 18;
			minx = 0;
			miny = 0;
			maxx = 0;
			maxy = 0;
			while ((ch = *str++)) {
				// charBounds() modifies x/y to advance for each character,
				// and min/max x/y are updated to incrementally build bounding rect.
				display->charBounds(ch, &x, &y, &minx, &miny, &maxx, &maxy, font);
				display->drawChar(x,y,ch,display->colour565(255,0,0), font);
			}
		} else if (buffer[0] =='r' && buffer[1] == '#') {
			delete display;
			display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, PIN_RST, 240, 320, R90DEG);
			display->reset();
			display->init();
			display->clear();
		} else {
			memset(buffer, 0, sizeof(buffer));
		}
	}
}
