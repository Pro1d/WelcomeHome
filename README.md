# Welcome Home
My solution to make my home be smart and cozy.


## Features
 * Music player with remote control: raspotify, Youtube
 * Video player with remote control: read video from Youtube or other VOD platforms
 * Remote light switch and plug control, automatic light switch off
 * "Export" my public IP address
 * Smartphone as remote controller
 * Voice synthesis for alert messages
 * Sweet led matrix to display with snake game animation, time and int/ext temperature


## To Do Features 
 * Shutter remote
 * More autonomous and smart behaviour of all the aforementioned features


## Tools, dependencies and requirements
 * [Arduino ATmega2456](https://store.arduino.cc/arduino-mega-2560-rev3) with [PlatformIO](https://platformio.org/) tools
 * Some electronic stuff...
 * Wireless Local Area Network
 * Raspberry Pi 3
 * Python3 
    * Main libraries: pyserial, [rpyc](https://rpyc.readthedocs.io/en/latest/) 3.4.4, http.server
    * [Google Drive python API](https://developers.google.com/drive/v3/web/about-sdk)
 * systemd services
 * [HTTP-shortcuts](https://github.com/Waboodoo/HTTP-Shortcuts) widgets for Android

![Electric diagram](arduino.png "Summary of modules with input/output arduino pins")
