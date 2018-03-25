# Welcome Home
My solution to make my home "smart" and more confortable.

## Features
 * Music server
 * Light switch
 * "Export" my public IP address
 * ...

## To Do Features 
 * Shutter remote
 * Ambiance lighting remote control
 * More autonomous and smart behaviour of all the aforementioned features.


## Tools, dependencies and requirements

 * Arduino ATmega2456 with PlatformIO
 * Some electronic stuff...
 * Old laptop converted in a local 24/24H server
    * LUbuntu 17.10
    * GUI terminal disabled via systemctl (lightdm.service)
    * disable suspend when close lid: /etc/systemd/logind.conf > HandleLidSwitch=ignore
 * Python3 
    * pip3 pyserial
    * pip3 rpyc
    * pip3 platformio
    * pip3 netifaces
    * http.server
    * urllib.parse
    * Google Drive python API
 * ServerX dependent music software runs with xvfb-run
 * tmux for window splitting with SSH

