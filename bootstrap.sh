#!/bin/bash
# raspotify
curl -sL https://dtcooper.github.io/raspotify/install.sh | sh
# mpg123 audio player
sudo apt install mpg123
# python
sudo apt install python3 python3-pip
sudo pip3 install rpyc pyserial
# platformio core (CLI)
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py
python3 get-platformio.py
# export PATH=$PATH:~/.platformio/penv/bin
# serial port usermod
sudo usermod -a -G dialout $USER

#sudo pip install youtube-dl
#sudo pip3 install google-api-python-client oauth2client plumbum==1.6.
# todo: google api keys
