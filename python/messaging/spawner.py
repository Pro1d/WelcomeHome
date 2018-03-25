#!/usr/bin/python3
import rpyc_client
import argparse
import subprocess
import shlex
import os
import sys


def get_script_path():
    return os.path.dirname(os.path.realpath(sys.argv[0]))


class Spawner(object):
    def __init__(self, port):
        self._port_params = ['--port', str(port)]
        self._client = rpyc_client.Client("spawner",
                                          callback=self.receiveCallback,
                                          port=port)
        self._tasks_list = {}
        self._dev_null = open(os.devnull, 'w')

    def load_task_list(self, filename):
        with open(filename) as f:
            for line in f.readlines():
                if line[0] != '#':
                    args = shlex.split(line)
                    task_name = args[0]
                    self._tasks_list[task_name] = args[1:]
    
    def get_task(self, task_name):
        if task_name in self._tasks_list:
            return self._tasks_list[task_name]
        else:
            return None

    def get_task_list(self):
        return self._tasks_list

    def start_task(self, task_name, task_params):
        subprocess.Popen(["python3", get_script_path()+"/task.py", task_name]
                         + self._port_params + task_params,
                         stdout=self._dev_null, stderr=self._dev_null)

    def stop_task(self, task_name):
        self._client.sendMsg(task_name, "exit")

    def start_all(self):
        clients = set(self._client.getAllClients())
        launch = set(self._tasks_list.keys()).difference(clients)
        for t in launch:
            self.start_task(t, self._tasks_list[t])

    def receiveCallback(self, dtype, sender, data): 
        words = data.split()
        if words != 2:
            return False

        task_name, action = words
        if task_name not in self._tasks_list:
            return False

        # check task_name state
        # send command action to task name
        # action: start, stop
        if action == 'start':
            self.start_task(task_name, self._tasks_list[task_name])
        elif action == 'stop':
            self.stop_task(task_name)
        else:
            return False

        return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", type=str,
                        default=get_script_path()+"/tasks_list.txt",
                        help="File containing the input list of task")
    parser.add_argument("-l", "--launch-all", dest="launch_all",
                        action="store_true",
                        help="Launch all tasks")
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    args = parser.parse_args()

    spawner = Spawner(args.port)
    spawner.load_task_list(args.file)
    if args.launch_all:
        spawner.start_all()

    print('Type "command_name [start]|stop" or "command_name args".')
    print('Print known tasks:\n> list')
    print('Task from list example:\n> spotifake [start]')
    print('Custom task example:\n> custom --period 60 touch file.txt')
    print('Stop task:\n> custom stop')
    print('')

    try:
        while True:
            command = shlex.split(input('> '))
            if len(command) == 0:
                continue
            name, args = command[0], command[1:]
            # Known task start/stop
            if name == 'list' and len(args) == 0:
                for n, c in spawner.get_task_list().items():
                    print('{}: {}'.format(n, c))
            elif args == ['stop']:
                spawner.stop_task(name)
            elif args in ([], ['start']):
                params = spawner.get_task(name)
                if params is None:
                    print('Unknown task "{}"'.format(name))
                else:
                    spawner.start_task(name, params)
            # Custom task
            else:
                spawner.start_task(name, args)
    except EOFError:
        pass
    except KeyboardInterrupt:
        pass
