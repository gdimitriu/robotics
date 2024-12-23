from machine import Pin
import utime
led_external = Pin(5, Pin.OUT)
led_external.low()
led_internal = Pin("LED", Pin.OUT)
led_internal.low()
while True:
    led_external.toggle()
    led_internal.toggle()
    print("Toggle")
    utime.sleep(1)