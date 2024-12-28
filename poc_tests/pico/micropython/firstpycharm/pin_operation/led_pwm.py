import time
from machine import Pin, PWM


def run():
    pwm = PWM(Pin(15))
    # Set the PWM frequency.
    pwm.freq(1000)
    # Fade the LED in and out a few times.
    duty = 0
    direction = 1
    for _ in range(8 * 256):
        duty += direction
        if duty > 255:
            duty = 255
            direction = -1
        elif duty < 0:
            duty = 0
            direction = 1
        pwm.duty_u16(duty * duty)
        time.sleep(0.01)
