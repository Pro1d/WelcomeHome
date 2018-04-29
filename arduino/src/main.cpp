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

//DelayedDigitalOutput ddout;
LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2> toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;

void on_button_pushed() {
  toggleLight.toggle();
}

void print_debug() {
  int l1 = analogRead(ANALOG_LUMINOSITY_PIN1);
  int l2 = analogRead(ANALOG_LUMINOSITY_PIN2);
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
  serial.init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_DETECTION_STATUS, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
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
  
  int l1 = analogRead(ANALOG_LUMINOSITY_PIN1);
  int l2 = analogRead(ANALOG_LUMINOSITY_PIN2);
  digitalWrite(LIGHT_DETECTION_STATUS, l1-l2 > 100 ? HIGH : LOW);
}
