https://playground.arduino.cc/Learning/CommandLine

platformio init --board megaatmega2560
platformio run
platformio run -t upload
platformio device monitor

sudo usermod -a -G dialout $USER
