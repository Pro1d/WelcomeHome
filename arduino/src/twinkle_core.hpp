#pragma once

#include <Arduino.h>

#define H               6
#define T               100
#define A0              110
#define INTENSITY_MAX   255

enum Mode { UNSTABLE, BLAZING };

template <int PWM_PIN>
class TwinkleCore {
public:
  void init() {
    pinMode(PWM_PIN, OUTPUT);
    frame = 0;
    last_t = 0;
    
    const int prime[H] = {2,3,5,7,11,13};
    for(int i = 0; i < H; i++) {
      ampl[i] = 0;
      period[i] = prime[i];
    }
    
    intensity = A0;
    actual_intensity = 0;
    mode = UNSTABLE;
  }
  void setMode(Mode m) {
    mode = m;
  }
  void update() {
    int t = millis();

    if(t - last_t < T)
      return;

    last_t = t;

    intensity = 0;
    switch(mode) {
      case BLAZING:
        for(int i = 0; i < H; i++) {
          int p = frame % period[i];
          if(p == 0) {
            if((frame / period[i]) % 2 == 0) {
              intensity += (INTENSITY_MAX - A0 + (H - 1)) / H;
            }
          }
        }
        intensity += A0;
        break;
      case UNSTABLE:
        for(int i = 0; i < H; i++) {
          int p = frame % period[i];
          if(p == 0) {
            if((frame / period[i]) % 2 == 0) {
              intensity++;
              ampl[i] = 1;
            } else {
              ampl[i] = 0;
            }
          }
        }
        if(ampl[0] && ampl[1] && ampl[3] && !(ampl[5] && ampl[4] && ampl[2]))
          intensity = (intensity + H) * 2;
        else if(ampl[4] && (ampl[3] || ampl[2]))
          intensity += H;
        intensity = intensity * INTENSITY_MAX / (H*3) + A0/4;
        break;
    }

    //actual_intensity += intensity;
    //actual_intensity /= 2;
    actual_intensity = intensity;
    analogWrite(PWM_PIN, actual_intensity);
    frame++;
  }
private:
  Mode mode;
  int frame;
  int last_t;
  int intensity;
  int actual_intensity;
  int ampl[H];
  int period[H];
};
