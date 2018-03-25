#!/usr/bin/python3
import rpyc
import argparse
from rpyc.utils.server import ThreadedServer


class Server(rpyc.Service):

    CLIENTS = {}
    CONNECTIONS = {}
    def printClientsList(self):
        print("\nConnected clients:")
        if self.CLIENTS:
            for name in self.CLIENTS:
                print('    '+name)
        else:
            print('    -')
        print('')

    def on_connect(conn):
        conn.CONNECTIONS[conn] = None
        print("Connected: {}".format(conn))

    def on_disconnect(conn):
        print("Disconnected: {}".format(conn))
        if conn in conn.CONNECTIONS:
            clientID = conn.CONNECTIONS[conn]
            if clientID in conn.CLIENTS:
                print("Disconnected: {}".format(clientID))
                del conn.CLIENTS[clientID]
                conn.printClientsList()
            del conn.CONNECTIONS[conn]

    def exposed_registerClient(self, clientID):
        print("Registered: {} {}".format(clientID, self))
        self.CLIENTS[clientID] = self
        self.CONNECTIONS[self] = clientID
        self.printClientsList()

    def exposed_getAllClients(self):
        return self.CLIENTS.keys()

    def exposed_sendDataToClient(self, clientID, dataType, sender, data):
        if clientID in self.CLIENTS:
            self.CLIENTS[clientID]._conn.root.exposed_catchData(dataType,
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
    t = ThreadedServer(Server, port = args.port,
                       protocol_config={"allow_public_attrs" : True})
    print("Start server 0.0.0.0:{}".format(args.port))
    t.start()
