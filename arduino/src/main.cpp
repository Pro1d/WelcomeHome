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
#include "twinkle_core.hpp"

void blink_led() {
  Serial.println("Blink built-in LED");

  blink<LED_BUILTIN, 500>();
}

//DelayedDigitalOutput ddout;
LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2> toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;
TwinkleCore<LIGHT_DETECTION_STATUS> twinkle_core;

bool cmd_toggle_light = false;

void on_button_pushed() {
  cmd_toggle_light = true;
}

void print_debug() {
  int l1 = analogRead(ANALOG_HIGH_LUMINOSITY_PIN);
  int l2 = analogRead(ANALOG_LOW_LUMINOSITY_PIN);
  int t = analogRead(ANALOG_TEMPERATURE_PIN);
  Serial.print("L: ");
  Serial.print(l1, DEC);
  Serial.print(" ");
  Serial.print(l2, DEC);
  Serial.print(" T: ");
  Serial.println(t, DEC);
}

void setup()
{
  randomSeed(analogRead(10));
  serial.init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
  twinkle_core.init();
  // Push button for light
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_PIN), on_button_pushed, FALLING);
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
      case DEBUG:
        print_debug();
        valid_command = true;
        break;
      default:
        break;
    }

    if(!valid_command) {
      Serial.print("Unknown command: (");
      Serial.write(cmd.target);
      Serial.print(", ");
      Serial.write(cmd.action);
      Serial.println(")");
    }
  }

  twinkle_core.update();

  if(cmd_toggle_light) {
    cmd_toggle_light = false;
    toggleLight.toggle();
  }
  
  int l1 = analogRead(ANALOG_HIGH_LUMINOSITY_PIN);
  int l2 = analogRead(ANALOG_LOW_LUMINOSITY_PIN);
  twinkle_core.setMode(l1-l2 > 100 ? BLAZING : UNSTABLE);
}
