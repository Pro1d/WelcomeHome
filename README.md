# Welcome Home
My solution to make my home be smart and cozy.


## Features
 * Music server with remote control
 * Remote light switch
 * "Export" my public IP address
 * Smartphone as remote controller


## To Do Features 
 * Shutter remote
 * Ambiance lighting remote control
 * More autonomous and smart behaviour of all the aforementioned features


## Tools, dependencies and requirements
 * [Arduino ATmega2456](https://store.arduino.cc/arduino-mega-2560-rev3) with [PlatformIO](https://platformio.org/) tools
 * Some electronic things...
 * Wireless Local Area Network
 * Old laptop converted to a local server (low performance but also low energy consumption)
    * LUbuntu 17.10
    * GUI terminal disabled via `systemctl` (`lightdm.service`)
    * Disabled suspend on lid closed: `/etc/systemd/logind.conf` -> `HandleLidSwitch=ignore`
 * Python3 
    * Main libraries: pyserial, [rpyc](https://rpyc.readthedocs.io/en/latest/) 3.4.4, http.server
    * [Google Drive python API](https://developers.google.com/drive/v3/web/about-sdk)
 * ServerX dependent music software runs with `xvfb-run`
 * tmux for window splitting with SSH
 * [HTTP-shortcuts](https://github.com/Waboodoo/HTTP-Shortcuts) widgets for Android
