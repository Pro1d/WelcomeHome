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
                                 timeout=5,  # read timeout
                                 write_timeout=5, # write timeout
                                 bytesize=bytesize,
                                 parity=parity,
                                 stopbits=stopbits)
        self.ser.setPort(fn)

        # connected, disconnected, connecting, stopping
        self._state = "connecting"
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
        self._state = "stopping"
        self._state_lock.release()
        self._write_thread.join()
        self._read_thread.join()
        self._state_thread.join()

    def connect(self):
        self._state_lock.acquire()
        if self._state == "disconnected":
            self._state = "connecting"
        self._state_lock.release()

    def disconnect(self):
        self._state_lock.acquire()
        if self._state == "connecting" or self._state == "connected":
            self._state = "disconnected"
        self._state_lock.release()

    def get_state(self):
        return self._state

    def state_loop(self):
        print("Thread state", file=sys.stderr)
        while True:
            self._state_lock.acquire()
            print("State: ", self._state, file=sys.stderr)
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
                    #print(e)
                    self.ser.close()
                if self.ser.isOpen():
                    self._state = "connected"
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    time.sleep(5)
            elif self._state == "connected":
                if not self.ser.isOpen():
                    self._state = "connecting"
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    time.sleep(5)
            elif self._state == "disconnected":
                if self.ser.isOpen():
                    self.ser.close()
                    self._state_lock.release()
                else:
                    self._state_lock.release()
                    time.sleep(5)
            else: # stopping
                self.ser.close()
                self._state = "stopping"
                self._state_lock.release()
                break
        
    def write(self, data):
        if self._state == "connected":
            self._write_queue.put(data)

    def write_loop(self):
        print("Thread write", file=sys.stderr)
        while self._state != "stopping":
            try:
                data = self._write_queue.get(timeout=5)
                if self._state == "connected":
                    self.ser.write(data.encode("latin-1"))
                self._write_queue.task_done()
            except serial.serialutil.SerialException as e:
                # Connection lost ->  reconnecting
                self._state_lock.acquire()
                self._state = "connecting"
                self._state_lock.release()
                time.sleep(5)
            except queue.Empty:
                pass

    def read_line(self):
        if self._state == "connected" or not self._read_queue.empty():
            data = self._read_queue.get()
            self._read_queue.task_done()
            return data
        else:
            return None

    def read_loop(self):
        print("Thread read", file=sys.stderr)
        while self._state != "stopping":
            if self._state == "connected":
                try:
                    line = self.ser.readline()
                    if line:
                        self._read_queue.put(line[:-2].decode())
                except serial.serialutil.SerialException as e:
                    # Connection lost ->  reconnecting
                    self._state_lock.acquire()
                    self._state = "connecting"
                    self._state_lock.release()
                    time.sleep(5)
            else:
                time.sleep(5) # wait for reconnection
