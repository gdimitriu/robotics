import machine
import time

led_pin = machine.Pin("LED", machine.Pin.OUT)  # GPIO pin 25 controls the onboard LED


while True:
    led_pin.on()  # Toggle the LED state
    print("led on")
    time.sleep(1)    # Wait for 1 second
    led_pin.off()
    print("led off")
    time.sleep(1)
