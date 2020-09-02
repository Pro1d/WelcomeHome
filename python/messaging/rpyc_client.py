#!/usr/bin/python3
import rpyc
from rpyc.utils.helpers import classpartial
import sys
import time

class MessagingClientService(rpyc.Service):
    exposed_clientID = ""

    def __init__(self, clientID, cb):
        self.exposed_clientID = clientID
        self._callback = cb

    def exposed_catchData(self, dataType, sender, data):
        if self._callback is not None:
            self._callback(dataType, sender, data)

class Client(object):

    def __init__(self, name, callback=None, address="localhost", port=18812):
        self._name = name
        conn = None
        while conn is None:
            try:
                service = classpartial(MessagingClientService, name, callback)
                conn = rpyc.connect(address, port, service=service)
                self._conn = conn
            except Exception as e:
                print(e, file=sys.stderr)
                print("Retry in 5sec...")
                time.sleep(5)

        rpyc.BgServingThread(self._conn)

    def sendMsg(self, dest, data):
        try:
            return self._conn.root.sendDataToClient(dest, "plain-text",
                                                    self._name, data)
        except Exception as e:
            print(e, file=sys.stderr)
            return False

    def getAllClients(self):
        return self._conn.root.getAllClients()
