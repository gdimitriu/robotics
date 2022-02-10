/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <pico/stdlib.h>

int main() {
    stdio_init_all();
    for (int i = 0; i < 10; i++) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
    printf("Please input data\n");
	fflush(stdout);
    char data[255];
	scanf("%s",&data);
	fflush(stdin);
	printf("Received %s\n",data);
    printf("Please input data for quit write exit\n");
	fflush(stdout);
	while(strcmp(data,"exit") != 0) {
		scanf("%s",&data);
		fflush(stdin);
		printf("Received %s\n",data);
		fflush(stdout);
	}
    return 0;
}
