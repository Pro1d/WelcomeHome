#!/usr/bin/python3
import rpyc
import argparse
from rpyc.utils.server import ThreadedServer


class MessagingService(rpyc.Service):
    def __init__(self):
        self.CLIENTS = {}
        self.CONNECTIONS = {}

    def printClientsList(self):
        print("Connected clients:")
        if self.CLIENTS:
            for name in self.CLIENTS:
                print('    '+name)
        else:
            print('    -')
        print('')

    def on_connect(self, conn):
        clientID = conn.root.clientID
        self.CLIENTS[clientID] = conn
        self.CONNECTIONS[conn] = clientID
        self.printClientsList()
        print("Connected: {}".format(clientID))

    def on_disconnect(self, conn):
        if conn in self.CONNECTIONS:
            clientID = self.CONNECTIONS[conn]
            del self.CLIENTS[clientID]
            del self.CONNECTIONS[conn]
            print("Disconnected: {}".format(clientID))
            self.printClientsList()

    def exposed_getAllClients(self):
        return self.CLIENTS.keys()

    def exposed_sendDataToClient(self, clientID, dataType, sender, data):
        if clientID in self.CLIENTS:
            self.CLIENTS[clientID].root.exposed_catchData(dataType,
                                                                sender,
                                                                data)
            return True
        else:
            return False


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()
    t = ThreadedServer(MessagingService(), port = args.port,
                       protocol_config={"allow_public_attrs" : True})
    print("Start server 127.0.0.1:{}".format(args.port))
    t.start()
