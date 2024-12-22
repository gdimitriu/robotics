from machine import Pin

counter = 0
pin = Pin(5, Pin.IN, Pin.PULL_UP)
while True:
    if pin.value() == 0:
        print("Button Pressed")
        counter += 1
        print("Count={}".format(counter))
