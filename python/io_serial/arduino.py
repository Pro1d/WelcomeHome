#!/usr/bin/python3
from io_serial.device_serial import Serial
from messaging.rpyc_client import Client
import argparse
import time
import threading
import os
import sys
import struct
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
    "plug1":   'P',
    "plug2":   'Q',
}
COMMANDS = {
    "blink": set(["trigger"]),
    "light": set(["toggle"]),
    "tetris": set(["on", "off", "toggle"]),
    "debug": set(["trigger"]),
    "serial": set(["on", "off"]),
    "clock": set(), #"auto"
    "plug1": set(["on", "off", "toggle"]),
    "plug2": set(["on", "off", "toggle"]),
}

HEADER = "$"
bHEADER = HEADER.encode()

SERIAL_MSG_DESC = {
        # HEADER+TYPE:
        #   type, (fmt cf. struct + S for uint16+string), (field names)
        bHEADER+b'E':
            ('event',   '<cI',  ('subtype', 'data32')),
        bHEADER+b'D':
            ('debug',   '<S',    ('text',)),
        bHEADER+b'S':
            ('sensors', '<??hh', ('light_on', 'day_light', 'luminosity_high', 'luminosity_low')),
}
# Event Subtype list
EST_AUTO_LIGHT_OFF = b'L'
EST_SNAKE_HIGH_SCORE = b'H'
EST_SNAKE_SCORE = b'S'

def messageSegmentation(data):
    i = 0
    msgs = []
    while i+2 <= len(data):
        # seek to begin of next message
        while data[i:i+2] not in SERIAL_MSG_DESC:
            i += 1
            if i+2 > len(data):
                return (msgs, i)
        
        # save the current index, if the message is incomplete, the next
        # messageSegment will start at this index
        begin = i

        # build format
        name, fmt, fields = SERIAL_MSG_DESC[data[i:i+2]]
        endianness = fmt[0]
        fmt = fmt.split('S')
        for f in range(0, len(fmt)-1):
            fmt[f] = fmt[f]+'H' # add uint16 before each 'S'
        i += 2
        
        # unpack
        pack = []
        for f in range(len(fmt)):
            if f > 0: # add endianness and '{n}s' for char[n]
                strlen = pack[-1]
                del pack[-1]
                fmt[f] = endianness+str(strlen)+'s'+fmt[f]
            # check data length
            size = struct.calcsize(fmt[f])
            if i+size > len(data):
                return (msgs, begin)
            else:
                pack += [*struct.unpack(fmt[f], data[i:i+size])]
            i += size

        # add new message and update cursor
        msgs.append(dict(zip(fields, pack)))
        msgs[-1]['type'] = name

    return (msgs, i)

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
            rpc_client.sendMsg("tts", "Connecting to serial")
        elif args["action"] == "off":
            ser.disconnect()
            rpc_client.sendMsg("tts", "Serial disconnected")
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

def write_snake_score(score):
    with open('/home/pi/snake_score.txt', 'a') as f:
        f.write(str(score)+'\n')

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("-f", "--file", default="/dev/ttyACM0",
                        help="Serial file name, default is /dev/ttyACM0. "
                             "See also /dev/serial/by-id/*")
    args = parser.parse_args()


    print("Serial port:", args.file, file=sys.stderr)
    ser = Serial(args.file, baudrate=115200)
    rpc_client = Client("arduino", callback=receiveCallback, port=args.port)

    # publish clock
    threading.Thread(target=publish_clock).start()

    # main loop / read message from serial
    try:
        buf = b''
        while True:
            data = ser.read()
            if data is not None:
                buf += data
                msgs, cursor = messageSegmentation(buf)
                buf = buf[cursor:]
                for m in msgs:
                    if m['type'] == "debug":
                        print(format_time() + " " + m['text'].decode())
                    elif m['type'] == "sensors":
                        pass
                    elif m['type'] == "event":
                        if m['subtype'] == EST_AUTO_LIGHT_OFF:
                            rpc_client.sendMsg("tts", "Automatic switch off")
                        elif m['subtype'] == EST_SNAKE_HIGH_SCORE:
                            if 9 <= time.localtime().tm_hour < 23:
                                rpc_client.sendMsg("tts", "The new high score is {}! Congratulation".format(m['data32']))
                            write_snake_score(m['data32'])
                        elif m['subtype'] == EST_SNAKE_SCORE:
                            write_snake_score(m['data32'])
            else:
                time.sleep(1)
    except KeyboardInterrupt:
        print("Stopping...", file=sys.stderr)
        ser.stop()
        os._exit(0)
