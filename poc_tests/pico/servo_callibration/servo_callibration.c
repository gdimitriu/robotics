/*
 * this is my intepretation of 
 * https://www.youtube.com/watch?v=fCIFYKFNcGc
 */
#include <stdio.h>
#include <pico/stdlib.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pico/time.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>

#define SERVO_PIN 22
#define SERIAL_DEBUG_MODE true
#define SERVO_180 3200
#define SERVO_135 2600
#define SERVO_90 1850
#define SERVO_45 1100
#define SERVO_0 500
/* does not work as expected
uint32_t pwm_set_freq_duty(uint slice_num, uint chan,uint32_t f, int d) {
	uint32_t clock = 125000000;
	uint32_t divider16 = clock / f / 4096 + 
                           (clock % (f * 4096) != 0);
	if (divider16 / 16 == 0)
		divider16 = 16;
	uint32_t wrap = clock * 16 / divider16 / f - 1;
	pwm_set_clkdiv_int_frac(slice_num, divider16/16, divider16 & 0xF);
	pwm_set_wrap(slice_num, wrap);
	pwm_set_chan_level(slice_num, chan, wrap * d / 100);
	return wrap;
}
*/

void setMillis(int servoPin, float millis) {
	pwm_set_gpio_level(servoPin, (millis/20000.f)*29062.f);
}

void setServo(int servoPin, float startMillis) {
	gpio_set_function(servoPin, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(servoPin);
	pwm_config config = pwm_get_default_config();
	pwm_config_set_clkdiv(&config, 64.f);
	pwm_config_set_wrap(&config, 29062.f);
	pwm_init(slice_num, &config, true);
	setMillis(servoPin, startMillis);
}

bool direction = true;
int currentMillis = 400;
bool isRunContinous = false;

void runContinous() {
	currentMillis += (direction)? 5 : -5;
	if ( currentMillis >= 2400 )
		direction = false;
	if ( currentMillis <= 400 )
		direction = true;
	setMillis(SERVO_PIN, currentMillis);
	sleep_ms(10);
}

int main() {
	char buffer[256];
	
	stdio_usb_init();
	stdio_flush();
	sleep_ms(5000);
#ifdef SERIAL_DEBUG_MODE
	printf("Starting...\n");
	fflush(stdout);
#endif
	/* does not work as expected !!!
	gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
	// Figure out which slice we just connected
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
	uint chan = pwm_gpio_to_channel(SERVO_PIN);
    pwm_set_freq_duty(slice_num,chan, 50, 75);	
	pwm_set_enabled(slice_num, true);
	pwm_set_gpio_level(SERVO_PIN, 0); */
	setServo(SERVO_PIN, 0);
	while (1) {
		printf("Input Command\n");
		fflush(stdout);
		scanf("%s",buffer);
		fflush(stdin);
		printf("received: <<%s>>\n",buffer);
		fflush(stdout);
		if ( strlen(buffer) > 2 ) {
			if ( buffer[0] == 't' ) {
				char *current = &buffer[1];
				buffer[strlen(buffer) -1] = '\0';
				float value = atof(current);
				setServo(SERVO_PIN, value);
			} else if ( buffer[0] == 'd' ) {
				char *current = &buffer[1];
				buffer[strlen(buffer) -1] = '\0';
				int value = atoi(current);
				if ( value == 180 ) {
					setServo(SERVO_PIN, SERVO_180 );
				} else if ( value == 135 ) {
					setServo(SERVO_PIN, SERVO_135 );
				} else if ( value == 90 ) {
					setServo(SERVO_PIN, SERVO_90 );
				} else if ( value == 45 ) {
					setServo(SERVO_PIN, SERVO_45 );
				} else if ( value == 0 ) {
					setServo(SERVO_PIN, SERVO_0 );
				} else 
				if ( value < 180 && value > 135 ) { 
					setServo(SERVO_PIN, SERVO_135 + (value - 135)* 13.33);
				} else if ( value < 135 && value > 90) {
					setServo(SERVO_PIN, SERVO_90 + (value - 90) * 16.66);
				} else if ( value > 45 && value < 90 ) {
					setServo(SERVO_PIN, SERVO_45 + (value - 45) * 16.66);
				} else if ( value > 0 && value < 45 ) {
					setServo(SERVO_PIN, SERVO_0 + value * 13.33);
				}
			}
			isRunContinous = false;
		} else if ( buffer[0] == 'c' ) {
			runContinous();
			isRunContinous = true;
		}
	}
}
