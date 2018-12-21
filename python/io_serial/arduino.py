#!/usr/bin/python3
from io_serial.device_serial import Serial
from messaging.rpyc_client import Client
import argparse
import time
import threading
import os
from queue import Queue

cmd_queue = Queue(32)

ACTIONS = {
    "off":     '0',
    "on":      '1',
    "toggle":  '~',
    "trigger": 't',
    "auto":    'a',
}
TARGETS = {
    "blink":   'B',
    "tetris":  'T',
    "light":   'L',
    "debug":   'D',
    "clock":   'C',
}
COMMANDS = {
    "blink": set(["trigger"]),
    "light": set(["toggle"]),
    "tetris": set(["on", "off", "toggle"]),
    "debug": set(["trigger"]),
    "serial": set(["on", "off"]),
    "clock": set(), #"auto"
}
HEADER = "$"
DISCONNECT = 0
CONNECT = 1

def receiveCallback(dtype, sender, data):
    msg = None
    try:
        domain, args = eval(data)
        assert domain in COMMANDS, "Unknown domain"
        assert args["action"] in COMMANDS[domain], "Unknown action"
        if domain in TARGETS:
            msg = TARGETS[domain]+ACTIONS[args["action"]]
        print("Received:", domain, args["action"])
    except:
        print("Invalid message:", data)
    
    if msg is not None:
        cmd_queue.put(HEADER+msg)
        return True
    elif domain == "serial":
        cmd_queue.put({"on":CONNECT, "off":DISCONNECT}[args["action"]])
        return True
    return False


def read_serial(ser):
    while True:
        try:
            if ser.is_open():
                print(time.ctime().split()[3], ser.read_line()[:-2].decode())
            else:
                time.sleep(1)
        except:
            pass

def publish_clock():
    while True:
        time.sleep(60)
        t = time.localtime()
        cmd_queue.put(HEADER+TARGETS["clock"]+ACTIONS["auto"]
                +"%02d%02d%02d" % (t.tm_hour, t.tm_min, t.tm_sec))


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("-f", "--file", default="/dev/ttyACM0",
                        help="Serial file name, default is /dev/ttyACM0")
    args = parser.parse_args()

    ser = Serial(args.file)
    c = Client("arduino", callback=receiveCallback, port=args.port)

    print("Serial port:", args.file)

    # read serial
    threading.Thread(target=read_serial, args=(ser,)).start()

    threading.Thread(target=publish_clock).start()

    # Send command
    try:
        while True:
            cmd = cmd_queue.get()
            if cmd == CONNECT:
                ser.connect()
                print("Open serial")
            elif cmd == DISCONNECT:
                ser.disconnect()
                print("Close serial")
            elif ser.is_open():
                ser.write(cmd)
                print("Write on serial: "+cmd)
            else:
                print("Serial is disconnected")
            cmd_queue.task_done()
    except KeyboardInterrupt:
        os._exit(0)
