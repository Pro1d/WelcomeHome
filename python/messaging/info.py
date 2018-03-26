#!/usr/bin/python3
import argparse
from rpyc_client import Client


NAME = "__info__"

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()
    try:
        c = Client(NAME, port=args.port)
    except:
        print("Server unreachable on port", args.port)
        exit(0)

    print("Server running on port", args.port) 

    clients = c.getAllClients()
    if len(clients) > 1:
        for client_name in clients:
            if client_name != NAME:
                print("  -", client_name)
    else:
        print("    [No client connected]")

    exit(0)
