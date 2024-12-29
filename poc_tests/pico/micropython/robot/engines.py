from machine import Pin, PWM


def init_engines():
    print("InitEngines")


def go(leftSpeed=0, rightSpeed=0):
    print("Go(%g,%g)" % leftSpeed, rightSpeed)


def break_engines():
    print("BreakEngines")
