# the pio rx is from
# https://github.com/micropython/micropython/blob/master/examples/rp2/pio_uart_rx.py
#
# Example using PIO to create a UART RX interface.
#
# To make it work you'll need a wire connecting GPIO4 and GPIO3.
#
# Demonstrates:
#   - PIO shifting in data on a pin
#   - PIO jmp(pin) instruction
#   - PIO irq handler


# ruff: noqa: F821 - @asm_pio decorator adds names to function scope

from machine import Pin
from rp2 import StateMachine, asm_pio
import rp2
import time

UART_BAUD = 9600

PIO_RX_PIN = Pin(9, Pin.IN, Pin.PULL_UP)


@asm_pio(
    in_shiftdir=rp2.PIO.SHIFT_RIGHT,
)
def uart_rx():
    # fmt: off
    label("start")
    # Stall until start bit is asserted
    wait(0, pin, 0)
    # Preload bit counter, then delay until halfway through
    # the first data bit (12 cycles incl wait, set).
    set(x, 7)                 [10]
    label("bitloop")
    # Shift data bit into ISR
    in_(pins, 1)
    # Loop 8 times, each loop iteration is 8 cycles
    jmp(x_dec, "bitloop")     [6]
    # Check stop bit (should be high)
    jmp(pin, "good_stop")
    # Either a framing error or a break. Set a sticky flag
    # and wait for line to return to idle state.
    irq(block, 4)
    wait(1, pin, 0)
    # Don't push data if we didn't see good framing.
    jmp("start")
    # No delay before returning to start; a little slack is
    # important in case the TX clock is slightly too fast.
    label("good_stop")
    push(block)
    # fmt: on


# The handler for a UART break detected by the PIO.
def handler(sm):
    print("break", time.ticks_ms(), end=" ")


# Set up the state machine we're going to use to receive the characters.
sm = StateMachine(
    0,
    globals()["uart_rx"],
    freq=8 * UART_BAUD,
    in_base=PIO_RX_PIN,  # For WAIT, IN
    jmp_pin=PIO_RX_PIN,  # For JMP
)
sm.irq(handler)
sm.active(1)
SR05EN = Pin(8, Pin.OUT)
SR05EN.high()
def get_distance():
    global sm
    global SR05EN
    H_DATA = 0
    L_DATA = 0
    SUM = 0
    SR05EN.low()
    S_DATA = sm.get() >> 24
    if S_DATA == 0xFF:
        H_DATA = sm.get() >> 24
        L_DATA = sm.get() >> 24
        SUM = sm.get() >> 24
    SR05EN.high()
    if S_DATA == 0xFF and H_DATA == 0xAA and L_DATA == 0xAA and SUM == 0x53:
        print("Distance more than 2000 mm\n")
        return
    distance = H_DATA * 256 + L_DATA
    sum = S_DATA + H_DATA + L_DATA
    sum = sum & 0xFF
    if SUM == sum:
        print("The distance is : %u mm\n" % distance)
        return distance
    print(S_DATA, H_DATA, L_DATA, SUM, sum, distance)
    return 2000

def readcontinous():
    while True:
        get_distance()
        time.sleep_ms(20)
