/*
 * hc-sr04.cpp
 *
 *  Created on: Jan 16, 2023
 *      Author: Gabriel Dimitriu 2023
 */

#include <stdio.h>
#include <pico/stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pico/time.h>

#define ECHO_PIN 19
#define TRIG_PIN 18

int main() {
	char buffer[256];
	char lcdBuffer[256];
	
	
	stdio_init_all();
	stdio_flush();
	sleep_ms(5000);
	
	printf("Starting before gpio init\n");
	fflush(stdout);
	gpio_init(TRIG_PIN);
	gpio_set_dir(TRIG_PIN, GPIO_OUT);
	gpio_init(ECHO_PIN);
	gpio_set_dir(ECHO_PIN, GPIO_IN);
	gpio_pull_up(ECHO_PIN);
	
	printf("Starting\n");
	fflush(stdout);
	scanf("%s", buffer);
	fflush(stdin);
	printf("Receiving start command=%s\n", buffer);
	fflush(stdout);
	printf("Starting SR04\n");
	fflush(stdout);
	printf("Enter d# to get the value from SR04\n");
	fflush(stdout);
	while (1) {
		printf("Input Command\n");
		fflush(stdout);
		scanf("%s",buffer);
		fflush(stdin);
		printf("received: <<%s>>\n",buffer);
		fflush(stdout);
		
		if (buffer[0] == 'd' && buffer[1] == '#') {
			uint64_t start;
			memset(lcdBuffer, 0, sizeof(lcdBuffer));
			long currentDistance = 4500;
			bool timeoutOccured = false;
			printf("time0 = %" PRIu64 " %" PRIu64 "\n", start, time_us_64());
			fflush(stdout);
			gpio_put(TRIG_PIN, false);
			busy_wait_us(2);
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
			if (timeoutOccured == false) {				
				int64_t microseconds = time_us_64() - start;
				currentDistance = ((microseconds/2)/29.1) * 10; //cm * 10 to be mm
			}
			sprintf(lcdBuffer, "Distance = %ld mm", currentDistance);
			printf("time2 = %" PRIu64 " %" PRIu64 "\n", start, time_us_64());
			fflush(stdout);

			printf("%s\n",lcdBuffer);
			fflush(stdout);
		} else {
			memset(buffer, 0, sizeof(buffer));
		}
	}
}
