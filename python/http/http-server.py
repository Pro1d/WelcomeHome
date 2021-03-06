#!/usr/bin/python3

from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import sys
import subprocess
import os
import argparse
from messaging.rpyc_client import Client


msg_client = None

def read_last_line(filename):
    with open(filename, 'rb') as f:
        f.seek(-2, os.SEEK_END) 
        while f.read(1) != b'\n':
            f.seek(-2, os.SEEK_CUR) 
        return f.readline().decode()[:-1]
    return None

class RequestHandler(BaseHTTPRequestHandler):

    def _parse_request(self):
        parsed_req = urlparse(self.path)
        args = parse_qs(parsed_req.query)
        if self.headers.get('content-type', '') \
            == 'application/x-www-form-urlencoded':
                body = self.rfile.read(int(self.headers.get('content-length')))
                args = parse_qs(body)

        args = dict((k, v[0]) for k, v in args.items())

        return (parsed_req.path, args)

    def do_POST(self):
        path, args = self._parse_request()
        if path is not None:
            self.do('POST', path, args)

    def do_GET(self):
        path, args = self._parse_request()
        if path is not None:
            self.do('GET', path, args)

    def do(self, method, path, args):
        retval = None
        data = None
        path = path.split('/')[1:]
        if len(path) == 2 and path[0] == "mega":
            if msg_client.sendMsg("arduino", str([path[1], args])):
                retval = 0
            else:
                retval = 1
        elif len(path) == 1 and path[0] == "tts" and "text" in args:
            lang = [args["lang"]] if "lang" in args else []
            if msg_client.sendMsg("tts", str(lang + [args["text"]])):
                retval = 0
            else:
                retval = 1
        #elif len(path) == 2 and path[0] == "player":
        #    if msg_client.sendMsg("mediaplayer", str((path[1], args))):
        #        retval = 0
        #    else:
        #        retval = 1
        elif len(path) == 2 and path[0] == 'system':
            if os.geteuid() != 0:
                retval = 1
            else:
                retval = 0

                if path[1] == 'shutdown':
                    msg_client.sendMsg("tts", "System shutdown")
                    subprocess.Popen(['shutdown', 'now'])
                elif path[1] == 'reboot':
                    msg_client.sendMsg("tts", "System reboot")
                    subprocess.Popen(['reboot'])
                else:
                    retval = 1
        elif path == ['sensor']:
            data = read_last_line("/home/pi/sensors.txt")+'\n'
            retval = 0 if data else 1

        if retval is None:
            self.send_error(400, 'Bad Request')
        elif retval == 0:
            self.send_response(200)
            self.send_header("Content-type", "text/plain;charset=utf-8")
            self.end_headers()
            if data:
                self.wfile.write(data.encode("utf-8"))
        else:
            self.send_error(500, 'Trigger command failed')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("-P", "--http-port", type=int, default=8080,
                        dest='http_port',
                        help="Set http port number, default is 8080")
    args = parser.parse_args()
    if args.http_port < 1024 and os.geteuid() != 0:
        sys.stderr.write('Port {} (< 1024) requires root privileges.\n'
                         .format(args.http_port))
        sys.exit(-2)

    msg_client = Client("http-server", port=args.port)
    server = HTTPServer(("0.0.0.0", args.http_port), RequestHandler)
    print("Run http server on port", args.http_port)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    sys.exit(0)
