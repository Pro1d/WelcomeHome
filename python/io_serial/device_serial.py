#!/usr/bin/env python

import serial
import sys
import time
import termios

class Serial:
    #  bytesize=8, parity='N', stopbits=1
    def __init__(self, fn="/dev/ttyACM0", baudrate=9600, bytesize=8, parity='N', stopbits=1):
        self.ser = serial.Serial(fn, baudrate, timeout=None,
                                 bytesize=bytesize,
                                 parity=parity,
                                 stopbits=stopbits)

        # Avoid arduino auto reset on seril connection
        fd = self.ser.fileno()
        attr = termios.tcgetattr(fd)
        attr[2] &= ~termios.HUPCL
        termios.tcsetattr(fd, termios.TCSANOW, attr)

        # Open and write data
        self.connect()

    def connect(self):
        self.ser.close()
        self.ser.open()

    def disconnect(self):
        self.ser.close()

    def is_open(self):
        return self.ser.isOpen()

    def write(self, data):
        return self.ser.write(data.encode('latin-1'))


    def read(self, size=1):
        return self.ser.read(size=size)


    def read_line(self):
        return self.ser.readline()


    def close(self):
        self.ser.close()
