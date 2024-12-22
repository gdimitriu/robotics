from machine import Pin
import time

interrupt_flag = 0
debounce_time = 0
pin = Pin(5, Pin.IN, Pin.PULL_UP)
led = Pin("LED", Pin.OUT)
count = 0


def callback(pin):
    global interrupt_flag, debounce_time
    if (time.ticks_ms() - debounce_time) > 500:
        interrupt_flag = 1
        debounce_time = time.ticks_ms()


pin.irq(trigger=Pin.IRQ_FALLING, handler=callback)

while True:
    if interrupt_flag is 1:
        interrupt_flag = 0
        print("Interrupt Detected")
        led.toggle()
