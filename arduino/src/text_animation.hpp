#pragma once

#include "font.hpp"
#define TEXT_PERIOD   150

template <int ROW, int COL>
class TextAnimation {
  public:
    void init() {
      cursor = 0;
      text_len = 0;
      playing = false;
      last_frame = 0;
    }

    template <const int _RO[ROW], const int _CO[COL]>
    void update(Matrix<ROW, COL, _RO, _CO>& matrix) {
      if(!playing)
        return;

      const unsigned long t = millis();
      if(t - last_frame < TEXT_PERIOD)
        return;

      last_frame = t;

      matrix.shift_left();

      if(cursor < text_len) {
        // Read next char
        if(column_cursor == 0) {
          fetch_char(text[cursor]);
        }
        // Print next column of the current char
        else {
          unsigned int shift = cur_char_width - column_cursor;
          for(int r = 0; r < ROW; r++)
            matrix.set_value(r, 0, (cur_char[r] >> shift) & 0x1);
        }

        // Update column/cursor
        column_cursor++;
        if(column_cursor > cur_char_width) { // char width + 1, for character space
          column_cursor = 0;
          cursor++;
        }
      }
      else {
        // Keep going until matrix is clear
        cursor++;
        if(cursor - text_len > COL)
          playing = false;
      }

    }

    bool is_playing() {
      return playing;
    }

    void start(const String& text) {
      start(text.c_str());
    }
    void start(const char* text) {
      column_cursor = 0;
      cursor = 0;
      text_len = 0;
      const char* in = text;
      char* buf = this->text;
      while(*in != '\0') {
        *buf = *in;
        text_len++;
        in++;
        buf++;
      }
      playing = true;
    }

  private:
    void fetch_char(char c) {
      int offset = ((int) c) * (1 + ROW);
      cur_char_width = pgm_read_byte_near(characters5xN + offset);
      for(int r = 0; r < ROW; r++) {
        ++offset;
        cur_char[r] = pgm_read_byte_near(characters5xN + offset);
      }
    }

    unsigned long last_frame;
    unsigned int cursor;
    unsigned int text_len;
    bool playing;
    byte column_cursor;
    byte cur_char_width;
    byte cur_char[ROW];
    char text[128];
};
