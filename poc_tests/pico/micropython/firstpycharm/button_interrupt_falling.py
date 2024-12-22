from machine import Pin

interrupt_flag = 0
pin = Pin(5, Pin.IN, Pin.PULL_UP)
counter = 0


def callback(pin):
    global interrupt_flag
    interrupt_flag = 1


pin.irq(trigger=Pin.IRQ_FALLING, handler=callback)
while True:
    if interrupt_flag is 1:
        print("Interrupt has occured")
        interrupt_flag = 0
        counter += 1
        print("Count={}".format(counter))
