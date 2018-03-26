#!/usr/bin/env python

import serial
import sys
import time
import termios

class Serial:
    #  bytesize=8, parity='N', stopbits=1
    def __init__(self, fn="/dev/ttyACM0", baudrate=9600, bytesize=8, parity='N', stopbits=1):
        self.ser = serial.Serial(fn, baudrate, timeout=0.1,
                                 bytesize=bytesize,
                                 parity=parity,
                                 stopbits=stopbits)

        # Avoid arduino auto reset on seril connection
        fd = self.ser.fileno()
        attr = termios.tcgetattr(fd)
        attr[2] &= ~termios.HUPCL
        termios.tcsetattr(fd, termios.TCSANOW, attr)

        # Open and write data
        self.ser.close()
        self.ser.open()


    def write(self, data):
        return self.ser.write(data.encode('latin-1'))


    def read(self, size=1):
        return self.ser.read(size=size)


    def close(self):
        self.ser.close()