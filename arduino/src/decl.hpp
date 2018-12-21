#pragma once

#define MATRIX_LED_ROWS 5
#define MATRIX_LED_COLS 7

#define ANALOG_HIGH_LUMINOSITY_PIN  0 // A0
#define ANALOG_LOW_LUMINOSITY_PIN   1 // A1
#define ANALOG_TEMPERATURE_PIN  2 // A2
#define PUSH_BUTTON_PIN         2
#define TONE_PIN                3
#define BAD_THRESHOLD_BUTTON_PIN 6
#define MATRIX_LED_ROW          {24, 22, 23, 27, 28}
#define MATRIX_LED_COLUMN       {32, 30, 26, 25, 29, 31, 33}
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
  CLOCK = 'C',
};
enum Action : char {
  OFF = '0',
  ON = '1',
  TOGGLE = '~',
  TRIGGER = 't',
  AUTO = 'a'
};
