#!/usr/bin/python3
import rpyc_client
import argparse
import subprocess
import os
import time

proc = None
command = None
run_count = 0
last_run_date = 0.0
dev_null = open(os.devnull, 'w')
exit_task = False

def run_command():
    global proc, run_count, last_run_date
    proc = subprocess.Popen(command, stdout=dev_null, stderr=dev_null)
    run_count += 1
    last_run_date = time.time()
    print('Run command: '+" ".join(c if ' ' not in c else '"'+c+'"' for c in command))
    print('PID: %d'%proc.pid)

def receiveCallback(dtype, sender, data):
    global exit_task
    if proc is None:
        if data == 'start':
            run_command()
    else:
        if data == 'kill':
            proc.kill() # SIGKILL
        elif data == 'terminate':
            proc.terminate() # SIGTERM
        elif data == 'restart':
            proc.kill()
            run_command()
    if data == "exit":
        exit_task = True
        if proc is not None:
            proc.kill()
    elif data == 'state':
        print(proc, proc.poll() if proc else '-', run_count, exit_task)
    #proc.send_signal(...)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    # arg: --stdout --stderr --stdin
    # arg: --no-verbose
    parser.add_argument("-t", "--period", type=float, default=-1.0,
                        help="Auto rerun command at given period in seconds. 0.0 to restart immediately.")
    parser.add_argument("-e", "--disable-auto-exit", dest="auto_exit",
                        action="store_false",
                        help="Do not quit task when command terminates")
    parser.add_argument("-s", "--disable-auto-start", dest="auto_start",
                        action="store_false",
                        help="Do not start command when task starts")
    parser.add_argument("-q", "--quick-run", dest="quick_run",
                        action="store_true",
                        help="Smaller delay for faster run of short command. More precise period.")
    parser.add_argument("-p", "--port", type=int, default=18812,
                        help="Set port number, default is 18812")
    parser.add_argument("name", help="Name of the client.")
    parser.add_argument("command",  nargs="+", help="Command to run")
    args = parser.parse_args()

    command = args.command
    delay = 0.01 if args.quick_run else 1.0 

    c = rpyc_client.Client(args.name, callback=receiveCallback, port=args.port)

    if args.auto_start:
        run_command()

    while True: 
        if proc is not None:
            while proc.poll() is None:
                time.sleep(delay) # arg: --quick-run -> sleep(0.01)
            proc = None
            if not args.quick_run:
                last_run_date = time.time()
        elif exit_task or (args.auto_exit and run_count > 0 and args.period < 0):
            break
        elif args.period >= 0:
            remaining_time = args.period - (time.time() - last_run_date)
            if remaining_time <= 0:
                run_command()
            else:
                time.sleep(min(remaining_time, delay))
        else:
            time.sleep(delay)
