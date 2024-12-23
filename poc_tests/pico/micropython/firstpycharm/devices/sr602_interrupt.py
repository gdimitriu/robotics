from machine import Pin

interrupt_flag = 0
pin = Pin(5, Pin.IN, Pin.PULL_UP)
led = Pin(15, Pin.OUT)
led.low()
counter = 0
interrupt_off = 0
pin_state = 1

def callback(pin):
    global interrupt_flag
    global interrupt_off
    global pin_state
    pin_state ^= 1
    if pin_state is 1:
        interrupt_flag = 1
    else:
        interrupt_off = 1


pin.irq(trigger=Pin.IRQ_RISING | Pin.IRQ_FALLING, handler=callback)
while True:
    if interrupt_flag is 1:
        print("Interrupt has occured")
        interrupt_flag = 0
        counter += 1
        print("Count={}".format(counter))
        led.high()
    if interrupt_off is 1:
        print("Off")
        interrupt_off = 0
        led.low()
