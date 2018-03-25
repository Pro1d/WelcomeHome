/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "command_serial.hpp"

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif
#define LIGHT_COIL_1 11  
#define LIGHT_COIL_2 12  

void blink() {
  Serial.println("Blink built-in LED");

  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

bool light_toggle = false;
void toggleLight() {
  int pinHigh = light_toggle ? LIGHT_COIL_1 : LIGHT_COIL_2;

  digitalWrite(pinHigh, HIGH);
  delay(200);
  digitalWrite(pinHigh, LOW);

  light_toggle = !light_toggle;
}

void setup()
{
  CommandSerial::init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LIGHT_COIL_1, OUTPUT);
  pinMode(LIGHT_COIL_2, OUTPUT);
}

void loop()
{
  Command c = CommandSerial::readNextCommand();
  bool valid_command = false;

  switch(c.target) {
    case LED:
      if(c.action == TRIGGER) {
        blink();
        valid_command = true;
      }
      break;
    case LIGHT:
      if(c.action == TOGGLE) {
        toggleLight();
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
