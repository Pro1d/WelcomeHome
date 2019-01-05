#pragma once

#include "Arduino.h"
#include "decl.hpp"


class OutputSerial {
public:
  void init() {
    Serial.begin(SERIAL_BAUDRATE);
  }

  template <typename... Data>
  void send(Content type, Data... data) {
    header(type);
    pack(data...);
  }

private:
  void header(Content type) {
    Serial.write(SERIAL_HEADER_BYTE);
    Serial.write((uint8_t*)&type, sizeof(Content));
  }

  template <typename LastData>
  void pack(const LastData& data) {
    Serial.write((uint8_t*)&data, sizeof(LastData));
  }

  void pack(const char* text) {
    Serial.write(text);
  }

  template <typename FirstData, typename... NextData>
  void pack(const FirstData& first, NextData... next) {
    pack(first);
    pack(next...);
  }
};
