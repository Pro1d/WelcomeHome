#!/usr/bin/python3

from messaging.rpyc_client import Client
import argparse
import time
import subprocess
import os
import termios
import queue
import sys
import threading
import math
from omxcontrol import OmxControl


def volume2millibels(vol):
    return int(round(2000 * math.log10(vol)))


def millibels2volume(mB):
    return math.pow(10, mB/2000)


class Player(object):
    def __init__(self):
        self.is_paused = False
        self.millibels = -600
        self.playing_queue = []
        self.playing_index = 0
        self.fifo_filename = "/tmp/mp-fifo.{}".format(hex(abs(hash(time.time())))[2:])
        self.fifo = None
        self.player_run = True
        self.player_process = None
        self.omxcontrol_dbus = None
        self.player_append_event = threading.Event()
        self.player_playpause_event = threading.Event()
        self.player_thread = threading.Thread(target=self.player_loop)
        self.player_thread.start()

    def control(self, args):
        print("control", args)
        if 'action' not in args:
            return False
        elif args['action'] == 'playpause':
            self.playpause()
            self.omxplayer_control('playpause')
        elif args['action'] == 'stop':
            self.playing_index = 0
            self.playing_queue = []
            self.playpause(pause=False)
            self.omxplayer_control('exit')
        elif args['action'] == 'next' or args['action'] == 'skip':
            self.omxplayer_control('exit')
            self.playpause(pause=False)
        elif args['action'] == 'prev':
            self.playing_index = max(0, self.playing_index - 2)
            self.omxplayer_control('exit')
            self.player_append_event.set()
            self.playpause(pause=False)
        elif args['action'] == 'volume' and 'value' in args:
            self.millibels = (int(args['value']) - 80) * 30 # volume [0;100] -> mB
            self.omxplayer_control('volume')
        elif args['action'] == 'info':
            self.omxplayer_control('info')
        else:
            return False
        return True

    def omxplayer_control(self, action):
        if self.player_process is None or self.player_process.poll() is not None:
            return True
        dbus = OmxControl()
        
        if action == 'playpause':
            dbus.playPause()
        elif action == 'volume':
            dbus.volume(millibels2volume(self.millibels))
        elif action == 'info':
            dbus.action(OmxControl.ACTION_SHOW_INFO)
        elif action == 'exit':
            dbus.quit()
        else:
            return False
        return True

    def player_loop(self):
        while self.player_run:
            if self.playing_index >= len(self.playing_queue):
                print("Wait queue")
                self.playing_index = len(self.playing_queue)
                if self.playing_index == len(self.playing_queue):
                    self.player_append_event.wait()
            elif self.is_paused:
                print("Wait unpause")
                self.player_playpause_event.wait()
            else:
                self.player_append_event.clear()
                self.player_playpause_event.clear()
                media, fn = self.playing_queue[self.playing_index]
                print("Play {} [{}] {}/{}".format(fn, media, self.playing_index, len(self.playing_queue)))
                self.playing_index += 1
                self.play(media, fn)

    def play(self, media, filename):
        """ omxplayer fifo ||| youtube-dl -o - {videourl} > fifo
        omxplayer fifo ||| youtube-dl -x -o fifo {musicurl} """
        is_url = 'url' in media
        is_music = 'music' in media
        input_file = self.fifo_filename if is_url else filename
        
        self.player_process = subprocess.Popen(
                ['omxplayer', '-o', 'alsa', input_file,
                '--vol', str(self.millibels)],
                stdin=subprocess.DEVNULL,
                stdout=subprocess.DEVNULL,
                stderr=None) #subprocess.DEVNULL)

        if is_url:
            try:os.mkfifo(self.fifo_filename)
            except:pass  # fail if file already created
        
            # stuck until the fifo is open in read mode by omxplayer
            if not is_music:
                self.fifo = open(self.fifo_filename, 'w')

            downloader_process = subprocess.Popen(
                    ["youtube-dl", filename, '-o']
                    + ([self.fifo_filename, '--extract-audio']
                        if is_music
                        else ['-']),
                    stdin=subprocess.DEVNULL,
                    stdout=self.fifo if not is_music else None,
                    stderr=None) #subprocess.DEVNULL)
            if not is_music:
                downloader_process.wait()
                self.fifo.close()  # close stream, then omxplayer can stop

        self.player_process.wait()
        if is_music:  # workaround, youtube-dl doesn't stop after writing music to fifo
            downloader_process.terminate() 
    
    def playpause(self, pause=None):
        if pause is None:
            pause = not self.is_paused
        if pause != self.is_paused:
            print("Set pause:", pause)
            self.is_paused = pause
            self.player_playpause_event.set()

    def queue(self, data):
        print("Add to queue", data)
        self.playing_queue.append(data)
        self.player_append_event.set()

    def queue_file(self, path, append, mtype):
        index = len(self.playing_queue)
        
        if os.path.isdir(path):
            cmd = ''
            for f in os.listdir(path):
                file_path = os.path.join(path, f)
                self.queue((mtype, file_path))
        else:
            self.queue((mtype, path))

        if not append:
            self.playing_index = index
            self.playpause(pause=False)
            self.control({'action':'skip'})
   
    def queue_url(self, url, append, mtype):
        # TODO? append youtube playlist
        index = len(self.playing_queue)
        self.queue((mtype+' url', url))
        if not append:
            self.playing_index = index
            self.playpause(pause=False)
            self.control({'action':'skip'})

    def execute_command(self, data):
        args = data[1]
        if data[0] == 'control':
            return self.control(data[1])
        elif data[0] == 'video':
            append = args['append']=="1" if 'append' in args else False
            if 'url' in args:
                self.queue_url(args['url'], append, 'video')
                return True
            return False
        elif data[0] == 'music':
            append = args['append']=="1" if 'append' in args else False
            if 'url' in args:
                self.queue_url(args['url'], append, 'music')
            elif 'path' in args:
                self.queue_file(args['path'], append, 'music')
            else:
                return False
            return True
        else:
            return False

    def end_process(self):
        self.player_run = False
        self.player_append_event.set()
        self.player_playpause_event.set()
        self.omxplayer_control('exit')
        self.player_thread.join()
        if self.fifo is not None:self.fifo.close()
        try:os.remove(self.fifo_filename)
        except:pass

cmd_queue = queue.Queue(32)


def msgCallback(dtype, sender, data):
    target, args = eval(data)
    if target in ['control', 'music', 'video']:
        cmd = (target, args)
        cmd_queue.put(cmd)
        return True
    else:
        return False

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()

    player = Player()
    
    msg_client = Client("mediaplayer", callback=msgCallback, port=args.port)

    try:
        while True:
            cmd = cmd_queue.get()
            print(cmd)
            res = player.execute_command(cmd)
            if not res:
                print("Failed!")
            cmd_queue.task_done()
    except KeyboardInterrupt:
        pass

    player.end_process()
