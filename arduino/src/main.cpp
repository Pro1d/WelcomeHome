/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "latching_relay_control.hpp"
#include "command_serial.hpp"
#include "control.hpp"
#include "decl.hpp"

void blink_led() {
  Serial.println("Blink built-in LED");

  blink<LED_BUILTIN, 500>();
}

LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2> toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;

void setup()
{
  serial.init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
}

void loop()
{
  if(serial.readNextCommand(cmd, false)) {
    bool valid_command = false;

    switch(cmd.target) {
      case LED:
        if(cmd.action == TRIGGER) {
          blink_led();
          valid_command = true;
        }
        break;
      case LIGHT:
        if(cmd.action == TOGGLE) {
          toggleLight.toggle();
          valid_command = true;
        }
        break;
      case TETRIS:
        if(cmd.action == ON) {
          tetris.on();
          valid_command = true;
        } else if(cmd.action == OFF) {
          tetris.off();
          valid_command = true;
        }
        break;
      default:
        break;
    }

    if(!valid_command) {
      Serial.print("Unknown command: (");
      Serial.print(cmd.target);
      Serial.print(", ");
      Serial.print(cmd.action);
      Serial.println(")");
    }
  }
}
