from machine import Pin
import time
counter=0
debounce_time=0
pin = Pin(5, Pin.IN, Pin.PULL_UP)
while True:
    if ((pin.value() is 0) and (time.ticks_ms()-debounce_time) > 300):
        counter+=1
        debounce_time=time.ticks_ms()
        print("Button Pressed")
        print("Count={}".format(counter))