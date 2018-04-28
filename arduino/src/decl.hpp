#pragma once


#define TETRIS_COIL_SET     9
#define TETRIS_COIL_RESET   10
#define LIGHT_COIL_1        11  
#define LIGHT_COIL_2        12  
#ifndef LED_BUILTIN
#define LED_BUILTIN         13
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
  LIGHT = 'L'
};
enum Action : char {
  OFF = '0',
  ON = '1',
  TOGGLE = '~',
  TRIGGER = 't',
  AUTO = 'a'
};
