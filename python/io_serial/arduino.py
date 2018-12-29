#!/usr/bin/python3
from io_serial.device_serial import Serial
from messaging.rpyc_client import Client
import argparse
import time
import threading
import os
import sys
from queue import Queue

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
        ser.write(HEADER+msg)
        return True
    elif domain == "serial":
        if args["action"] == "on":
            ser.connect()
        elif args["action"] == "off":
            ser.disconnect()
        else:
            return False
        return True
    else:
        return False

def publish_clock():
    while True:
        time.sleep(60)
        t = time.localtime()
        ser.write(HEADER+TARGETS["clock"]+ACTIONS["auto"]
                +"%02d%02d%02d" % (t.tm_hour, t.tm_min, t.tm_sec))

def format_time():
    t = time.localtime()
    return "[%4d-%02d-%02d.%02d:%02d:%02d]" % (t.tm_year, t.tm_mon, t.tm_mday,
            t.tm_hour, t.tm_min, t.tm_sec)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("-f", "--file", default="/dev/ttyACM0",
                        help="Serial file name, default is /dev/ttyACM0. "
                             "See also /dev/serial/by-id/*")
    args = parser.parse_args()


    print("Serial port:", args.file, file=sys.stderr)
    ser = Serial(args.file)
    c = Client("arduino", callback=receiveCallback, port=args.port)

    # publish clock
    threading.Thread(target=publish_clock).start()

    # main loop / print serial in
    try:
        while True:
            txt = ser.read_line()
            if txt is not None:
                print(format_time(), txt)
            else:
                time.sleep(1)
    except KeyboardInterrupt:
        print("Stopping...", file=sys.stderr)
        ser.stop()
        os._exit(0)
