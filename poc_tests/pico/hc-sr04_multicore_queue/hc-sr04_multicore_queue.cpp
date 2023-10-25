/*
 * hc-sr04_multicore_queue.cpp
 *
 *  Created on: 2023
 *      Author: Gabriel Dimitriu 2023
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>
#include <string.h>

#define ECHO_PIN 19
#define TRIG_PIN 18


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

	stdio_init_all();
		
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);

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
		if (buffer[0] == 'd' && buffer[1] == '#') {
			uint32_t currentDistance;
			queue_entry_t entry;
			entry.func = &getDistance;
			queue_add_blocking(&call_queue, &entry);

			// We could now do a load of stuff on core 0 and get our result later
			printf("waiting %u  %u\n", queue_get_level(&call_queue), queue_get_level(&results_queue));
			fflush(stdout);
			queue_remove_blocking(&results_queue, &currentDistance);
			printf("Distance = %d mm", currentDistance);
			fflush(stdout);
		} else {
			memset(buffer, 0, sizeof(buffer));
		}
	}
}
