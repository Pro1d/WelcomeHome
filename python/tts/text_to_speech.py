#!/usr/bin/python3
import argparse
from messaging.rpyc_client import Client
from queue import Queue
import subprocess
import sys
import re

"""
Receive a string of a list of strings.
For a list, the first string can be "en", "fr" or "??-??" to specify a language
"""

text_queue = Queue(64)
CLIENT_NAME = "tts"
PLAYER_PRG = "/usr/bin/mplayer"
PLAYER_ARGS = "-ao alsa -really-quiet -noconsolecontrols -volume 50 -softvol"
LANG = {"en": "En-uk", "fr": "Fr-fr"}

def build_url(lang, txt):
    # "A0a" -> "%41%30%61"
    encoded = ''.join("%"+hex(ord(c))[2:].zfill(2) for c in txt.replace("\n", ""))
    url = "http://translate.google.com/translate_tts?ie=UTF-8&client=tw-ob&q={}&tl={}"
    return url.format(encoded, lang)

def onTextReceived(dtype, sender, data):
    lang = LANG["en"]
    try:
        l = eval(data)
        if isinstance(l, list) or isinstance(l, tuple):
            data = [*l]
    except:pass

    if isinstance(data, list):
        text = []
        for d in data:
            if isinstance(d, str):
                text.append(d)
            else:
                text = None
                break
        if text[0] in LANG:
            lang = LANG[text[0]]
            del text[0]
        elif len(text[0]) == 5 and re.match(r"[a-zA-Z]{2}-[a-zA-Z]{2}", text[0]) is not None:
            lang = text[0]
            del text[0]
    elif isinstance(data, str):
        text = [data]
    else:
        text = None

    if text is None:
        print("Non printable data: "+str(data), file=sys.stderr)
        return False

    for t in text:
        text_queue.put((lang, t))

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
            res = subprocess.call([PLAYER_PRG, *PLAYER_ARGS.split(), build_url(*txt)])
            if res != 0:
                print("Subprocess call failed! ({})".format(PLAYER_PRG), file=sys.stderr) 
            text_queue.task_done()
    except KeyboardInterrupt:
        pass
