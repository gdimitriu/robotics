/*
 * hc-sr04_multicore.cpp
 *
 *  Created on: Jan 17, 2023
 *      Author: Gabriel Dimitriu 2023
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <pico/multicore.h>
#include <string.h>
#include <inttypes.h>

#define ECHO_PIN 21
#define TRIG_PIN 20

static char event_str[128];

static const char *gpio_irq_str[] = {
        "LEVEL_LOW",  // 0x1
        "LEVEL_HIGH", // 0x2
        "EDGE_FALL",  // 0x4
        "EDGE_RISE"   // 0x8
};

void gpio_event_string(char *buf, uint32_t events) {
    for (uint i = 0; i < 4; i++) {
        uint mask = (1 << i);
        if (events & mask) {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0') {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events) {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}

static volatile uint64_t start = 0;
static volatile uint64_t finished = 0;
static volatile uint64_t edgeRise = 0;
static volatile uint64_t edgeFall = 0;
static volatile bool rise = false;
static volatile bool fall = false;
void gpio_callback(uint gpio, uint32_t events) {
	
	if (GPIO_IRQ_EDGE_RISE & events) {
		edgeRise = time_us_64();
		rise = true;
		//printf("rise %" PRIu64 "\n", edgeRise);
	}
	if (GPIO_IRQ_EDGE_FALL & events) {
		edgeFall = time_us_64();
		fall = true;
		//printf("fall %" PRIu64 "\n", edgeFall);
	}
	if (edgeFall == 0 && edgeRise == 0) {
		//gpio_event_string(event_str, events);
		//printf("00 %"  PRIu64 "%s\n", time_us_64(),event_str);
	} else {
		//gpio_event_string(event_str, events);
		//printf("xx %"  PRIu64 " %s\n", time_us_64(),event_str);
	}
	if (rise == true && fall == true) {
		gpio_event_string(event_str, events);
		printf("xx1 %" PRIu64 " %"  PRIu64 " %"  PRIu64 " %s\n", time_us_64(), edgeRise, edgeFall, event_str);
		rise = false;
		fall = false;
	}
}

void core1_entry() {
	while(1) {
		uint32_t (*func)() = (uint32_t(*)()) multicore_fifo_pop_blocking();
		uint32_t result = (*func)();
        multicore_fifo_push_blocking(result);
	}
}

uint32_t getDistance() {
	bool timeoutOccured = false;
	uint32_t currentDistance = 4500;
	gpio_pull_up(ECHO_PIN);	
	gpio_put(TRIG_PIN, false);
	busy_wait_us(20);
	gpio_put(TRIG_PIN, true);
	busy_wait_us(10);
	gpio_put(TRIG_PIN, false);
	while(gpio_get(ECHO_PIN) == false);
	start = time_us_64();
	while(gpio_get(ECHO_PIN) == true) {
		if ((time_us_64() - start) > 26190) {
			timeoutOccured = true;
			break;
		}
	}
	int64_t microseconds = 0;
	finished = time_us_64();
	if (timeoutOccured == false) {
		microseconds = finished - start;
		currentDistance = ((microseconds/2)/29.1) * 10; //cm * 10 to be mm
	}
	printf("Time %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", start, finished, time_us_64(), microseconds);
	return currentDistance;
}
int main() {
	char buffer[256];

	stdio_init_all();
	sleep_ms(5000);
	
	printf("Starting before gpio init\n");
	fflush(stdout);
	
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);
	gpio_pull_up(ECHO_PIN);	

	printf("Starting\n");
	scanf("%s", buffer);
	printf("Receiving start command=%s\n", buffer);
	multicore_reset_core1();
	multicore_launch_core1(core1_entry);
	gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
	printf("Starting HC-SR04_MULTICORE\n");
	printf("Enter d# to get the value from HC-SR04\n");
	while (1) {
		
		printf("Input Command\n");
		scanf("%s",buffer);
		fflush(stdin);			
		printf("received: <<%s>>\n",buffer);
		printf("Microsends0 %" PRIu64 "\n", edgeFall - edgeRise);
		printf("xx1 %" PRIu64 " %"  PRIu64 " %"  PRIu64 "\n", time_us_64(),edgeRise, edgeFall);
		if (buffer[0] == 'd' && buffer[1] == '#') {
			multicore_fifo_push_blocking((uintptr_t) &getDistance);
			uint32_t currentDistance = multicore_fifo_pop_blocking();
			printf("Distance = %u mm\n", currentDistance);
		}
		memset(buffer, 0, sizeof(buffer));
		printf("Microsends1 %" PRIu64 "\n", edgeFall - edgeRise);
	}
}
