#pragma once


#define ANALOG_LUMINOSITY_PIN1  0 // A0
#define ANALOG_LUMINOSITY_PIN2  1 // A1
#define ANALOG_TEMPERATURE_PIN  2 // A2
#define PUSH_BUTTON_PIN         2
#define LIGHT_DETECTION_STATUS  8
#define TETRIS_COIL_SET         9
#define TETRIS_COIL_RESET       10
#define LIGHT_COIL_1            11  
#define LIGHT_COIL_2            12  
#ifndef LED_BUILTIN
#define LED_BUILTIN             13
#endif

#define COMMAND_SERIAL_HEADER_BYTE (static_cast<char>('$'))

/**
 * tetris on/off
 * light toggle
 * shutter up/down/stop
 **/

enum Target : char {
  NONE = '_',
  LED = 'B',
  TETRIS = 'T',
  LIGHT = 'L',
  DEBUG = 'D',
};
enum Action : char {
  OFF = '0',
  ON = '1',
  TOGGLE = '~',
  TRIGGER = 't',
  AUTO = 'a'
};
