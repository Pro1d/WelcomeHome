#!/usr/bin/python3
from __future__ import print_function
import httplib2
import os
import sys
import argparse
import time
import requests

from apiclient.http import MediaFileUpload
from apiclient import discovery
from oauth2client import client
from oauth2client import tools
from oauth2client.file import Storage

update_credential = False

flags = None
if update_credential:
    flags = argparse.ArgumentParser(parents=[tools.argparser]).parse_args()

def get_script_path():
    return os.path.dirname(os.path.realpath(sys.argv[0]))

# If modifying these scopes, delete your previously saved credentials
# at ~/.credentials/drive-python-quickstart.json
SCOPES = 'https://www.googleapis.com/auth/drive'
CLIENT_SECRET_FILE = get_script_path()+"/"+'client_secret.json'
APPLICATION_NAME = 'Drive IP Share'

def get_public_ip():
    return requests.get('http://ip.42.pl/raw').text

def gen_time_file(fn, ip):
    with open(fn, 'r') as f:
        content = f.read()
        f.close()
    content = content.split('\n')
    content = [c.split() for c in content]
    if not content[-1]:del content[-1]
    if len(content) >= 2:
        if content[0][-1] == content[1][-1]:
            del content[0]
    content.insert(0, [time.ctime(), ip])
    content = [' '.join(c) for c in content]
    with open(fn, 'w') as f:
        f.write('\n'.join(content))
        f.close()

def get_credentials():
    """Gets valid user credentials from storage.

    If nothing has been stored, or if the stored credentials are invalid,
    the OAuth2 flow is completed to obtain the new credentials.

    Returns:
        Credentials, the obtained credential.
    """
    home_dir = os.path.expanduser('~')
    credential_dir = os.path.join(home_dir, '.credentials')
    if not os.path.exists(credential_dir):
        os.makedirs(credential_dir)
    credential_path = os.path.join(credential_dir,
                                   'drive-python-ip-share.json')

    store = Storage(credential_path)
    credentials = store.get()
    if not credentials or credentials.invalid:
        flow = client.flow_from_clientsecrets(CLIENT_SECRET_FILE, SCOPES)
        flow.user_agent = APPLICATION_NAME
        if flags:
            credentials = tools.run_flow(flow, store, flags)
        else: # Needed only for compatibility with Python 2.6
            credentials = tools.run(flow, store)
        print('Storing credentials to ' + credential_path)
    return credentials

def main():
    http = get_credentials().authorize(httplib2.Http())
    service = discovery.build('drive', 'v3', http=http)

    if not update_credential:
        parser = argparse.ArgumentParser()
        parser.add_argument("-p", "--ip", default=None)
        parser.add_argument("file_id")
        args = parser.parse_args()
        filename = get_script_path() + "/data" 
        ip = args.ip if args.ip is not None else get_public_ip()
        gen_time_file(filename, ip)
        service.files().update(body={"name":"ip_history.txt"}, fileId=args.file_id, media_body=MediaFileUpload(filename)).execute()

if __name__ == '__main__':
    main()

