#pragma once

#define EMPTY_5x3   0,0,0,0,0
#define UNKNOWN_5x3  EMPTY_5x3

// ASCII to 5x3 matrix
const PROGMEM byte characters5x3[] ={
  // skip 32 first non printable ascii char
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  // SPACE
  0b000,
  0b000,
  0b000,
  0b000,
  0b000,
  // !
  0b010,
  0b010,
  0b010,
  0b000,
  0b010,
  // "
  0b101,
  0b101,
  0b000,
  0b000,
  0b000,
  // #
  UNKNOWN_5x3,
  // $
  UNKNOWN_5x3,
  // %
  0b101,
  0b001,
  0b010,
  0b100,
  0b101,
  // &
  0b011,
  0b011,
  0b110,
  0b101,
  0b011,
  // '
  0b010,
  0b010,
  0b000,
  0b000,
  0b000,
  // (
  0b001,
  0b010,
  0b010,
  0b010,
  0b001,
  // )
  0b100,
  0b010,
  0b010,
  0b010,
  0b100,
  // *
  0b101,
  0b010,
  0b101,
  0b000,
  0b000,
  // +
  0b000,
  0b010,
  0b111,
  0b010,
  0b000,
  // ,
  0b000,
  0b000,
  0b000,
  0b010,
  0b100,
  // -
  0b000,
  0b000,
  0b111,
  0b000,
  0b000,
  // .
  0b000,
  0b000,
  0b000,
  0b000,
  0b010,
  // /
  0b001,
  0b001,
  0b010,
  0b100,
  0b100,
  // 0
  0b010,
  0b101,
  0b101,
  0b101,
  0b010,
  // 1
  0b010,
  0b110,
  0b010,
  0b010,
  0b111,
  // 2
  0b010,
  0b101,
  0b001,
  0b010,
  0b111,
  // 3
  0b110,
  0b001,
  0b110,
  0b001,
  0b110,
  // 4
  0b100,
  0b100,
  0b101,
  0b111,
  0b001,
  // 5
  0b111,
  0b100,
  0b110,
  0b001,
  0b110,
  // 6
  0b011,
  0b100,
  0b110,
  0b101,
  0b010,
  // 7
  0b111,
  0b001,
  0b010,
  0b100,
  0b100,
  // 8
  0b010,
  0b101,
  0b010,
  0b101,
  0b010,
  // 9
  0b010,
  0b101,
  0b011,
  0b001,
  0b110,
  // :
  0b000,
  0b010,
  0b000,
  0b010,
  0b000,
  // ;
  0b000,
  0b010,
  0b000,
  0b010,
  0b100,
  // <
  0b001,
  0b010,
  0b100,
  0b010,
  0b001,
  // =
  0b000,
  0b111,
  0b000,
  0b111,
  0b000,
  // >
  0b100,
  0b010,
  0b001,
  0b010,
  0b100,
  // ?
  0b110,
  0b001,
  0b010,
  0b000,
  0b010,
  // @
  0b110,
  0b001,
  0b010,
  0b101,
  0b010,
  // A
  0b010,
  0b101,
  0b111,
  0b101,
  0b101,
  // B
  0b110,
  0b101,
  0b110,
  0b101,
  0b110,
  // C
  0b011,
  0b100,
  0b100,
  0b100,
  0b011,
  // D
  0b110,
  0b101,
  0b101,
  0b101,
  0b110,
  // E
  0b111,
  0b100,
  0b110,
  0b100,
  0b111,
  // F
  0b111,
  0b100,
  0b110,
  0b100,
  0b100,
  // G
  0b011,
  0b100,
  0b101,
  0b101,
  0b010,
  // H
  0b101,
  0b101,
  0b111,
  0b101,
  0b101,
  // I
  0b111,
  0b010,
  0b010,
  0b010,
  0b111,
  // J
  0b001,
  0b001,
  0b001,
  0b101,
  0b010,
  // K
  0b101,
  0b101,
  0b110,
  0b101,
  0b101,
  // L
  0b100,
  0b100,
  0b100,
  0b100,
  0b111,
  // M
  0b111,
  0b111,
  0b101,
  0b101,
  0b101,
  // N
  0b101,
  0b111,
  0b111,
  0b111,
  0b101,
  // O
  0b010,
  0b101,
  0b101,
  0b101,
  0b010,
  // P
  0b110,
  0b101,
  0b110,
  0b100,
  0b100,
  // Q
  0b010,
  0b101,
  0b101,
  0b101,
  0b011,
  // R
  0b110,
  0b101,
  0b110,
  0b101,
  0b101,
  // S
  0b011,
  0b100,
  0b010,
  0b001,
  0b110,
  // T
  0b111,
  0b010,
  0b010,
  0b010,
  0b010,
  // U
  0b101,
  0b101,
  0b101,
  0b101,
  0b011,
  // V
  0b101,
  0b101,
  0b101,
  0b101,
  0b010,
  // W
  0b101,
  0b101,
  0b101,
  0b111,
  0b101,
  // X
  0b101,
  0b101,
  0b010,
  0b101,
  0b101,
  // Y
  0b101,
  0b101,
  0b010,
  0b010,
  0b010,
  // Z
  0b111,
  0b001,
  0b010,
  0b100,
  0b111,
  // [
  0b011,
  0b010,
  0b010,
  0b010,
  0b011,
  // BACK / 
  0b100,
  0b100,
  0b010,
  0b001,
  0b001,
  // ]
  0b110,
  0b010,
  0b010,
  0b010,
  0b110,
  // ^
  0b010,
  0b101,
  0b000,
  0b000,
  0b000,
  // _
  0b000,
  0b000,
  0b000,
  0b000,
  0b111,
  // `
  0b010,
  0b001,
  0b000,
  0b000,
  0b000,
  // a-z
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3, UNKNOWN_5x3,
  UNKNOWN_5x3, UNKNOWN_5x3,
  // {
  0b011,
  0b010,
  0b110,
  0b010,
  0b011,
  // |
  0b010,
  0b010,
  0b010,
  0b010,
  0b010,
  // }
  0b110,
  0b010,
  0b011,
  0b010,
  0b110,
  // ~
  0b000,
  0b100,
  0b111,
  0b001,
  0b000,
};