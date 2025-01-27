# Complete project details at https://RandomNerdTutorials.com/raspberry-pi-pico-servo-motor-micropython/

from machine import Pin, PWM
from time import sleep

# Set up PWM Pin for servo control
servo_pin = Pin(22)
servo = PWM(servo_pin)

# Set Duty Cycle for Different Angles
max_duty = 7864
min_duty = 1000
half_duty = int(max_duty / 2)

# Set PWM frequency
frequency = 50
servo.freq(frequency)


def move90Left():
    # Servo at 0 degrees
    servo.duty_u16(max_duty)


def move90Right():
    # servo at 180
    servo.duty_u16(min_duty)


def moveCenter():
    # servo at 90
    servo.duty_u16(half_duty)


def stop():
    servo.deinit()
