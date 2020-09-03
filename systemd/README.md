# Systemd

## Setup

Copy file or create links of `*.service` into `/etc/systemd/system`
`sudo cp -l *.service /etc/systemd/system
Then `sudo systemctl [--system] daemon-reload`.

## Start/Enable

Start the service right now:
`sudo systemctl start \*.service`

Auto start the service at system start-up:
`sudo systemctl enable \*.service`
