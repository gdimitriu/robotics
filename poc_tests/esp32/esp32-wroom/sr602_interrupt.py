"""
 esp32-wroom and interrupts

 Copyright 2025 Gabriel Dimitriu
  This file is part of robotics project.

 robotics is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 robotics is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with robotics; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
"""
from machine import Pin

interrupt_flag = 0
pin = Pin(23, Pin.IN, Pin.PULL_UP)
led = Pin(22, Pin.OUT)
led.value(0)
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
        led.value(1)
    if interrupt_off is 1:
        print("Off")
        interrupt_off = 0
        led.value(0)
