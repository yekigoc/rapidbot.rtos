import sys
sys.path.append('/usr/local/lib/python2.6/dist-packages/usb/')
import usb.core
import usb.util
from struct import *
from array import array

#import usb

# find our device
dev = usb.core.find(idVendor=0xEB03, idProduct=0x0920)

# was it found?
if dev is None:
    raise ValueError('Device not found')

dev.set_configuration()

CTRL_OUT = [0x81, 0x0]
dev.ctrl_transfer(0x81, 0x0, 0x07, 0x0, None)

