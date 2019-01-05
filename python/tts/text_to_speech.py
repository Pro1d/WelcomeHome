#!/usr/bin/python3
import argparse
from messaging.rpyc_client import Client
from queue import Queue
import subprocess
import sys


text_queue = Queue(64)
CLIENT_NAME = "tts"
PLAYER_PRG = "/usr/bin/mplayer"
PLAYER_ARGS = "-ao alsa -really-quiet -noconsolecontrols -volume 50"

def build_url(txt):
    # "A0a" -> "%41%30%61"
    encoded = ''.join("%"+hex(ord(c))[2:].zfill(2) for c in txt.replace("\n", ""))
    url = "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&q={}&tl=En-us"
    return url.format(encoded)

def onTextReceived(dtype, sender, data):
    if isinstance(data, list):
        text = []
        for d in data:
            if isinstance(d, str):
                text.append(d)
            else:
                text = None
                break
    elif isinstance(data, str):
        text = [data]
    else:
        text = None

    if text is None:
        print("Non printable data: "+str(data), file=sys.stderr)
        return False

    for t in text:
        text_queue.put(t)

    return True

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Text to speech. "
            "Read string (or list of strings) sent to this to this "
            "client '{}'".format(CLIENT_NAME))
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()

    c = Client(CLIENT_NAME, callback=onTextReceived, port=args.port)

    try:
        while True:
            txt = text_queue.get()
            print("Reading: {}".format(txt))
            res = subprocess.call([PLAYER_PRG, *PLAYER_ARGS.split(), build_url(txt)])
            if res != 0:
                print("Subprocess call failed! ({})".format(PLAYER_PRG), file=sys.stderr) 
            text_queue.task_done()
    except KeyboardInterrupt:
        pass
