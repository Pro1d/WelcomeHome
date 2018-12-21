#pragma once

#define CLOCK_PERIOD  (30uL * 1000uL)

class Clock {
  enum { H1, H2, M1, M2, S1, S2 };

public:
  void init() {
    for(int i = 0; i < 6; i++)
      time[i] = '0';
  }

  void synchronizeFromSerial() {
    last_frame = millis();
    Serial.readBytes(time, 6);
  }

  bool update() {
    unsigned long t = millis();
    if(t - last_frame < CLOCK_PERIOD)
      return false;
    unsigned long sec = (t - last_frame) / 1000;
    last_frame = t;
    
    unsigned int s = sec % 60;
    sec = sec / 60;
    unsigned int m = sec % 60;
    sec = sec / 60;
    unsigned int h = sec;
    
    time[S2] += s % 10;
    time[S1] += s / 10;
    if(time[S2] > '9') {
      time[S2] -= 10;
      ++time[S1];
    }
    if(time[S1] > '5') {
      time[S1] -= 6;
      ++time[M2];
    }

    time[M2] += m % 10;
    time[M1] += m / 10;
    if(time[M2] > '9') {
      time[M2] -= 10;
      ++time[M1];
    }
    if(time[M1] > '5') {
      time[M1] -= 6;
      ++time[H2];
    }

    int hours = (time[H1] - '0') * 10 + (time[H2] - '0') + h;
    while(hours > 23) {
      hours -= 24;
    }
    time[H2] = (hours % 10) + '0';
    time[H1] = (hours / 10) + '0';

    return true;
  }

  void getTimeF(char* txt) {
    txt[0] = time[H1];
    txt[1] = time[H2];
    txt[2] = ':';
    txt[3] = time[M1];
    txt[4] = time[M2];
    txt[5] = '\0';
  }
public:
  unsigned long last_frame;
  char time[6]; // HHMMSS
};
