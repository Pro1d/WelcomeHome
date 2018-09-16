#pragma once

#include "Arduino.h"
#include "control.hpp"
#include "decl.hpp"

#define LATCHING_RELAY_PULSE_DURATION  4 // ms

template<int PIN_SET, int PIN_RESET>
class LatchingRelayControl {
  public:
    void init(bool reset) {
      pinMode(PIN_SET, OUTPUT);
      pinMode(PIN_RESET, OUTPUT);
      is_on = false;
      if(reset) {
        write<PIN_RESET>();
      }
    }

    void on() {
      is_on = true;
      write<PIN_SET>();
    }

    void off() {
      is_on = false;
      write<PIN_RESET>();
    }

    void toggle() {
      is_on = !is_on;
      if(is_on)
        write<PIN_SET>();
      else
        write<PIN_RESET>();
    }

    void perform(Action a) {
      switch(a) {
        case OFF:
          off();
          break;
        case ON:
          on();
          break;
        case TOGGLE:
          toggle();
          break;
        default:
          break;
      }
    }

  private:
    template<int PIN>
    void write() {
      blink<PIN, LATCHING_RELAY_PULSE_DURATION>();
    }
    bool is_on;
};
