/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * modified by Gabriel Dimitriu 2021
 */

#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/gpio.h>

static char event_str[128];
#define LED_PIN 13
#define BUTTON_GPIO 12

/*
 * 3.3V to VCC rail
 * button to vcc rail and BUTTON_GPIO
 * led + longer pin to LED_PIN
 * led - shorter pin to 330 ohm resistor
 * resistor to GND rail
 * GND to raspberry pi pico GND
 */
void gpio_event_string(char *buf, uint32_t events);

void gpio_callback(uint gpio, uint32_t events) {
    // Put the GPIO event(s) that just happened into event_str
    // so we can print it
    gpio_event_string(event_str, events);
    printf("GPIO %d %s\n", gpio, event_str);
    if (GPIO_IRQ_EDGE_RISE == events) {
    	gpio_put(LED_PIN, 1);
    } else {
    	gpio_put(LED_PIN, 0);
    }
}

int main() {
    stdio_init_all();
    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_down(BUTTON_GPIO);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    printf("Hello GPIO IRQ\n");
    gpio_set_irq_enabled_with_callback(BUTTON_GPIO, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (1)
          tight_loop_contents();
    return 0;
}


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
