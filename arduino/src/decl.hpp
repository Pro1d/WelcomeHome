#pragma once

#define SERIAL_BAUDRATE 115200

#define MATRIX_LED_ROWS 5
#define MATRIX_LED_COLS 7

#define ANALOG_HIGH_LUMINOSITY_PIN  0 // A0
#define ANALOG_LOW_LUMINOSITY_PIN   1 // A1
#define ANALOG_TEMPERATURE_PIN  2 // A2
#define ANALOG_TEMPERATURE_INT_PIN  8 // A8
#define ANALOG_TEMPERATURE_EXT_PIN  9 // A9
#define PUSH_BUTTON_PIN         2
#define TONE_PIN                3
#define TRANSMITTER_433_PIN     7
#define LIGHT_DETECTION_STATUS  8
#define TETRIS_COIL_SET         9
#define TETRIS_COIL_RESET       10
#define LIGHT_COIL_1            11  
#define LIGHT_COIL_2            12  
#ifndef LED_BUILTIN
#define LED_BUILTIN             13
#endif
#define MATRIX_LED_ROW          {24, 22, 23, 27, 28}
#define MATRIX_LED_COLUMN       {32, 30, 26, 25, 29, 31, 33}

#define PRC_PERIOD               1648 // microseconds
#define PRC_DURATION_ONE         1212 // microseconds
#define PRC_DURATION_ZERO        392 // microseconds
#define PRC_BITS_ON              1,0,1,0
#define PRC_BITS_OFF             1,0,0,0
#define PRC_BITS_I1  0, 0,0,1,0, 1,0,1,0, 0,0,1,0, 1,0,1,0, 1,0,1,0
#define PRC_BITS_I2  0, 0,0,1,0, 1,0,1,0, 1,0,0,0, 1,0,1,0, 1,0,1,0

#define SERIAL_HEADER_BYTE (static_cast<char>('$'))

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
  PRC1 = 'P',
  PRC2 = 'Q',
};

enum Action : char {
  OFF = '0',
  ON = '1',
  TOGGLE = '~',
  TRIGGER = 't',
  AUTO = 'a'
};

enum Content : char {
  C_SENSORS = 'S', // '$S'+[sensor values]
  C_EVENT = 'E', // '$E'+Event+32bits data
  C_DEBUG = 'D' // '$D'+length+'str'
};

enum Event : char {
  E_AUTO_LIGHT_OFF = 'L',
  E_SNAKE_HIGH_SCORE = 'H',
  E_SNAKE_SCORE = 'S'
};
