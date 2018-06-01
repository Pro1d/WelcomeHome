#pragma once

#include "Arduino.h"

template <int ROW, int COL, const int ROW_PINS[ROW], const int COL_PINS[COL]>
class Matrix {
  public:
    void init() {
      for(int c = 0; c < COL; c++)
        pinMode(COL_PINS[c], OUTPUT);
      for(int r = 0; r < ROW; r++) {
        pinMode(ROW_PINS[r], OUTPUT);
        digitalWrite(ROW_PINS[r], HIGH);
        data[r] = 0x0;
      }
      cursor_row = 0;
    }

    void shift_left() {
      for(int r = 0; r < ROW; r++)
        data[r] <<= 1;
    }

    void shift_right() {
      for(int r = 0; r < ROW; r++)
        data[r] >>= 1;
    }

    void shift_up() {
      for(int r = 0; r < ROW-1; r++)
        data[r] = data[r+1];
      data[ROW-1] = 0x0;
    }

    void shift_down() {
      for(int r = ROW; --r;)
        data[r] = data[r-1];
      data[0] = 0x0;
    }

    void draw(byte* src) {
      for(int r = 0; r < ROW; r++)
        data[r] = src[r];
    }

    void clear(int r, int c) {
      data[r] &= ~(1 << c);
    }

    void set(int r, int c) {
      data[r] |= 1 << c;
    }

    void update() {
      byte row = data[cursor_row];

      // Turn off previous row
      digitalWrite(ROW_PINS[prev_cursor_row], HIGH);

      // Set each column
      for(int c = 0; c < COL; c++, row >>= 1)
        digitalWrite(COL_PINS[c], row & 0x1);

      // Turn on current row
      digitalWrite(ROW_PINS[cursor_row], LOW);

      // Move cursor for next update
      prev_cursor_row = cursor_row;
      cursor_row++;
      if(cursor_row >= ROW)
        cursor_row = 0;
    }

  private:
    int cursor_row, prev_cursor_row;
    byte data[ROW];
 };
