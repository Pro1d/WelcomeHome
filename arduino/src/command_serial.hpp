#pragma once

#include "Arduino.h"
#include "decl.hpp"


struct Command {
  Target target;
  Action action;
  Command() : target{NONE} {}
};

class CommandSerial {
public:
  static void init() {
    Serial.begin(9600);
  }

  static Command readNextCommand(bool blocking = true) {
    Command c;
    const int len = sizeof(Command);

    // Seek next header
    if(!seekHeader(blocking))
      return c;
    
    // Do not wait data if non blocking mode
    if(!blocking && Serial.available() < len)
      return c;

    // Wait for data
    while(Serial.available() < len)
      delay(1);

    // Read data
    Serial.readBytes(reinterpret_cast<char*>(&c), len);

    return c;
  }
private:
  static bool seekHeader(bool blocking = true) {
    char c;
    if(!blocking) {
      // Non blocking mode:
      //  Read available data until header is found
      while(Serial.available()) {
        Serial.readBytes(&c, 1);
        if(c == COMMAND_SERIAL_HEADER_BYTE)
          return true;
      }
      return false;
    }
    else {
      // Blocking mode:
      //  Wait and read data until header is found
      do {
        while(Serial.available() < 1)
          delay(1);
        Serial.readBytes(&c, 1);
      } while(c != COMMAND_SERIAL_HEADER_BYTE);
      return true;
    }
  }

};
