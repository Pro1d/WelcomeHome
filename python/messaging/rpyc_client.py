#!/usr/bin/python3
import rpyc
import sys
import time

class ClientService(rpyc.Service):
    def exposed_catchData(self, dataType, sender, data):
        if self._callback is not None:
            self._callback(dataType, sender, data)

    def set_callback(self, cb):
        self._callback = cb

class Client(object):

    def __init__(self, name, callback=None, address="localhost", port=18812):
        self._name = name
        conn = None
        while conn is None:
            try:
                conn = rpyc.connect(address, port=port, service=ClientService)
                self._conn = conn
            except Exception as e:
                print(e, file=sys.stderr)
                print("Retry in 5sec...")
                time.sleep(5)

        self._conn._local_root.set_callback(callback)
        self._conn.root.registerClient(name)
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
