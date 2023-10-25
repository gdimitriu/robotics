/*
 * fiming.cpp
 *
 *  Created on: Jan 16, 2023
 *      Author: Gabriel Dimitriu 2023
 */
#define PICO_TIME_DEFAULT_ALARM_POOL_DISABLED 0

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/timer.h>
#include <pico/multicore.h>
#include <string.h>

static volatile uint64_t start = 0;
static volatile uint64_t finished = 0;


void core1_entry() {
	while(1) {
		uint32_t (*func)() = (uint32_t(*)()) multicore_fifo_pop_blocking();
		uint32_t result = (*func)();
        multicore_fifo_push_blocking(result);
	}
}

uint32_t getTiming() {
	printf("time0 = %llu %llu\n", start, time_us_64());
	start = time_us_64();
	sleep_us(2);
	sleep_us(10);
	finished = time_us_64();
	printf("diff = %llu\n", finished - start);
	printf("time1 = %llu %llu %llu\n", start, time_us_64(), finished);
	return (finished - start);
}

int main() {
	char buffer[256];
	
	stdio_init_all();
	stdio_flush();
	printf("Starting\n");
	scanf("%[^\n]%*c", buffer);
	printf("Receiving start command=%s\n", buffer);
	printf("Starting timing\n");
	printf("Enter d# to get timing\n");
	multicore_reset_core1();
	multicore_launch_core1(core1_entry);
	memset(buffer, 0, sizeof(buffer));
	while (1) {
		printf("Input Command\n");
		scanf("%[^\n]%*c",buffer);
		printf("received: <<%s>>\n",buffer);
		
		if (buffer[0] == 'd' && buffer[1] == '#') {			
			multicore_fifo_push_blocking((uintptr_t) &getTiming);
			uint32_t currentTiming = multicore_fifo_pop_blocking();
			printf("Timing = %u mm\n", currentTiming);
		}
		memset(buffer, 0, sizeof(buffer));
	}
}
