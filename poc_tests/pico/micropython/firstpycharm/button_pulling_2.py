from machine import Pin
counter=0
pin = Pin(5, Pin.IN, Pin.PULL_DOWN)
while True:
    if pin.value() is 1:
        counter+=1
        print("Button Pressed")
        print("Count={}".format(counter))