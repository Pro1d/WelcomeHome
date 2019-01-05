#!/usr/bin/env python

import serial
import sys
import time
import termios
import threading
import queue
from queue import Queue

class Serial:
    #  bytesize=8, parity='N', stopbits=1
    def __init__(self, fn="/dev/ttyACM0", baudrate=9600, bytesize=8, parity='N', stopbits=1):
        self.ser = serial.Serial(port=None, # port not given here, thus serial is not open right now
                                 baudrate=baudrate,
                                 timeout=1,  # read timeout
                                 write_timeout=1, # write timeout
                                 bytesize=bytesize,
                                 parity=parity,
                                 stopbits=stopbits)
        self.ser.setPort(fn)

        # connected, disconnected, connecting, stopping
        self._state = None
        self._update_state("connecting", "Connecting...")
        self._state_lock = threading.Lock()

        self._write_queue = Queue(32)
        self._read_queue = Queue(32)
        self._state_queue = Queue(8)
        self._read_thread = threading.Thread(target=self.read_loop)
        self._write_thread = threading.Thread(target=self.write_loop)
        self._state_thread = threading.Thread(target=self.state_loop)
        self._read_thread.start()
        self._write_thread.start()
        self._state_thread.start()
    
    def stop(self):
        self._state_lock.acquire()
        self._update_state("stopping", "Disconnect and stop.")
        self._state_lock.release()
        self._write_thread.join()
        self._read_thread.join()
        self._state_thread.join()

    def connect(self):
        self._state_lock.acquire()
        if self._state == "disconnected":
            self._update_state("connecting", "Connecting...")
        self._state_lock.release()

    def disconnect(self):
        self._state_lock.acquire()
        if self._state == "connecting" or self._state == "connected":
            self._update_state("disconnected", "Disconnected")
        self._state_lock.release()

    def get_state(self):
        return self._state

    def _update_state(self, new_state, info=None):
        if new_state != self._state:
            if info:
                print(info)
            self._state = new_state

    def state_loop(self):
        while True:
            self._state_lock.acquire()
            if self._state == "connecting":
                try:
                    self.ser.close()
                    self.ser.open()
                    # Avoid arduino auto reset on serial connection
                    fd = self.ser.fileno()
                    attr = termios.tcgetattr(fd)
                    attr[2] &= ~termios.HUPCL
                    termios.tcsetattr(fd, termios.TCSANOW, attr)
                except serial.serialutil.SerialException as e:
                    self.ser.close()
                if self.ser.isOpen():
                    self._update_state("connected", "Connected!")
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    print("Cannot connect to serial, retry in 5 sec...")
                    time.sleep(5)
            elif self._state == "connected":
                if not self.ser.isOpen():
                    self._update_state("connecting", "Connection lost, reconnecting...")
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    time.sleep(1)
            elif self._state == "disconnected":
                if self.ser.isOpen():
                    self.ser.close()
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    time.sleep(1)
            elif self._state == "stopping":
                self.ser.close()
                self._state_lock.release()
                break
            else:
                break
        
    def write(self, data):
        if self._state == "connected":
            self._write_queue.put(data)

    def write_loop(self):
        while self._state != "stopping":
            try:
                data = self._write_queue.get(timeout=1)
                if self._state == "connected":
                    self.ser.write(data.encode())
                self._write_queue.task_done()
            except serial.serialutil.SerialException as e:
                # Connection lost ->  reconnecting
                self._state_lock.acquire()
                self._update_state("connecting", "Connection lost, reconnecting...")
                self._state_lock.release()
            except queue.Empty:
                pass

    def read(self, blocking=True):
        if self._state == "connected" or not self._read_queue.empty():
            try:
                data = self._read_queue.get(block=blocking)
                self._read_queue.task_done()
                return data
            except queue.Empty:
                return None
        else:
            return None

    def read_loop(self):
        while self._state != "stopping":
            if self._state == "connected":
                try:
                    # read 1 byte with blocking mode (and timeout)
                    data = self.ser.read(size=1)
                    # if not timeout, data is not empty
                    if data:
                        # then read all remaining OS buffer
                        data += self.ser.read_all()
                        self._read_queue.put(data)
                except serial.serialutil.SerialException as e:
                    # Connection lost ->  reconnecting
                    self._state_lock.acquire()
                    self._update_state("connecting", "Connection lost, reconnecting...")
                    self._state_lock.release()
                    time.sleep(1)
            else:
                time.sleep(1) # wait for reconnection
