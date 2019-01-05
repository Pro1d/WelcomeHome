#pragma once

#include "Arduino.h"
#include "decl.hpp"


struct Command {
  Target target;
  Action action;
};

class CommandSerial {
public:
  void init() {
    Serial.begin(SERIAL_BAUDRATE);
  }

  bool readNextCommand(Command& cmd, bool blocking = true) {
    char header;
    const int len = 1 + sizeof(Command);

    do {
      if(!blocking) {
        // Do not wait data if non blocking mode
        if(Serial.available() < len)
          return false;
      }
      else {
        // Wait for data
        while(Serial.available() < len)
          delay(1);
      }

      // Check header byte
      Serial.readBytes(&header, 1);
    
    } while(header != SERIAL_HEADER_BYTE);
    
    // Read data
    Serial.readBytes(reinterpret_cast<char*>(&cmd), sizeof(Command));

    return true;
  }

};
