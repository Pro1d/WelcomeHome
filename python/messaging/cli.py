#!/usr/bin/python3
import rpyc
import sys
import argparse
import rpyc_client

def receiveCallback(dtype, sender, data):
    print('\n'+sender+" ["+dtype+"]: "+data)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('name', type=str, nargs='?', default='dummy',
                                help='Name of the client')
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()
    c=rpyc_client.Client(args.name, callback=receiveCallback, port=args.port)

    print('Type "client message..." to send a message. Ctrl-C to quit.')
    print('Example: Bob Hello world!')
    print('')
    print('Your name is: '+args.name);

    try:
        while True:
            line = input('> ')
            dest = line[:line.find(' ')]
            msg = line[len(dest)+1:]
            result = c.sendMsg(dest, msg)
            if not result:
                sys.stderr.write('sendMsg failed\n')
    except KeyboardInterrupt:
        pass
    except EOFError:
        pass

    print('\nQuit.')
    exit(0)
