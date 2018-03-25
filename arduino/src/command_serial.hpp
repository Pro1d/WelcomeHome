#pragma once

#include "Arduino.h"

#define HEADER_BYTE (static_cast<char>('$'))

/**
 * tetris on/off
 * light toggle
 * shutter up/down/stop
 **/

enum Target : char {
  NONE,
  LED,
  TETRIS,
  LIGHT
};
enum Action : char {
  OFF,
  ON,
  TOGGLE,
  TRIGGER,
  AUTO
};

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
        if(c == HEADER_BYTE)
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
      } while(c != HEADER_BYTE);
      return true;
    }
  }

};
