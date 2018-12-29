#!/usr/bin/python3

from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import sys
import os
import argparse
from messaging.rpyc_client import Client


msg_client = None

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
        path = path.split('/')[1:]
        if len(path) == 2 and path[0] == "mega":
            if msg_client.sendMsg("arduino", str([path[1], args])):
                retval = 0
            else:
                retval = 1
        
        if retval is None:
            self.send_error(400, 'Bad Request')
        elif retval == 0:
            self.send_response(200)
            self.end_headers()
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

    msg_client = Client("http-server", args.port)
    server = HTTPServer(("0.0.0.0", args.http_port), RequestHandler)
    print("Run http server on port", args.http_port)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    sys.exit(0)
