#!/usr/bin/python3
from io_serial.device_serial import Serial
from messaging.rpyc_client import Client
import argparse
import time
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
}
COMMANDS = {
    "blink": set(["trigger"]),
    "light": set(["toggle"]),
    "tetris": set(["on", "off"]),
}
HEADER = "$"

def receiveCallback(dtype, sender, data):
    msg = None
    try:
        domain, args = eval(data)
        assert domain in COMMANDS, "Unknown domain"
        assert args["action"] in COMMANDS[domain], "Unknown action"
        msg = TARGETS[domain]+ACTIONS[args["action"]]
        print("Received:", domain, args["action"])
    except:
        print("Invalid message:", data)
    
    if msg is not None:
        cmd_queue.put(HEADER+msg)
        return True
    return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("-f", "--file", default="/dev/ttyACM0",
                        help="Serial file name")
    args = parser.parse_args()

    ser = Serial(args.file)
    c = Client("arduino", callback=receiveCallback, port=args.port)

    print("Serial port:", args.file)
    try:
        while True:
            cmd = cmd_queue.get()
            ser.write(cmd)
            cmd_queue.task_done()
    except KeyboardInterrupt:
        exit(0)
