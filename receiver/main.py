import board
import digitalio
from vs1053 import Player
from board import *
import busio

def getLed(pin):
    led = digitalio.DigitalInOut(pin)
    led.direction = digitalio.Direction.OUTPUT
    led.value = True
    return led

led = getLed(board.BLUE_LED)

print ("Play!")
spi = busio.SPI(D13, D12, D11)
player = Player(
    spi,
    xResetPin = board.A1,
    dReqPin = board.A2,
    xDCSPin = board.A3,
    xCSPin = board.A4,
    CSPin = board.D10
)
player.setVolume(1.0)

import esb
esb.init_rx()
while True:
    buf = esb.recv()
    if buf:
        led.value = True
        player.writeData(buf)
        led.value = False
