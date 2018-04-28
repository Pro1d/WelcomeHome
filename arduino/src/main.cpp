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

void setup()
{
  CommandSerial::init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
}

void loop()
{
  Command c = CommandSerial::readNextCommand();
  bool valid_command = false;

  switch(c.target) {
    case LED:
      if(c.action == TRIGGER) {
        blink_led();
        valid_command = true;
      }
      break;
    case LIGHT:
      if(c.action == TOGGLE) {
        toggleLight.toggle();
        valid_command = true;
      }
      break;
    case TETRIS:
      if(c.action == ON) {
        tetris.on();
        valid_command = true;
      } else if(c.action == OFF) {
        tetris.off();
        valid_command = true;
      }
      break;
    default:
      break;
  }

  if(!valid_command) {
    Serial.print("Unknown command: (");
    Serial.print(static_cast<int>(c.target), DEC);
    Serial.print(", ");
    Serial.print(static_cast<int>(c.action), DEC);
    Serial.println(")");
  }
}
