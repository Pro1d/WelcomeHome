#pragma once

#include "characters.hpp"
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

      unsigned long t = millis();
      if(t - last_frame < TEXT_PERIOD)
        return;

      last_frame = t;

      matrix.shift_left();

      unsigned int char_index = cursor / 4;
      unsigned int column = cursor % 4;
      unsigned int shift = 3 - column;

      if(char_index < text_len) {
        if(column == 0)
          fetch_char(text[char_index]);

        for(int r = 0; r < ROW; r++)
          matrix.set_value(r, 0, (cur_char[r] >> shift) & 0x1);
      }
      else if(cursor > COL + text_len * 4) {
        playing = false;
      }

      cursor++;
    }

    bool is_playing() {
      return playing;
    }

    void start(const String& text) {
      start(text.c_str());
    }
    void start(const char* text) {
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
      int offset = ((int) c) * ROW;
      for(int r = 0; r < ROW; r++)
        cur_char[r] = pgm_read_byte_near(characters5x3 + offset + r);
    }

    unsigned long last_frame;
    unsigned int cursor;
    unsigned int text_len;
    char text[128];
    bool playing;
    byte cur_char[ROW];
};
