#pragma once

// Class to send bit sequence with a 433MHz transmitter
template <int DATA_PIN,
         unsigned int PERIOD,
         unsigned int DURATION_ONE,
         unsigned int DURATION_ZERO>
class Transmitter433 {
public:
  void init() const {
    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(DATA_PIN, LOW);
  }
  
  template <bool B, bool... Bs>
  void send() const {
    digitalWrite(DATA_PIN, HIGH);
    delayMicroseconds(B ? DURATION_ONE : DURATION_ZERO);
    digitalWrite(DATA_PIN, LOW);
    delayMicroseconds(PERIOD - (B ? DURATION_ONE : DURATION_ZERO));
    
    send<Bs...>();
  }
private:
  template <typename EndOfReccursion=void>
  void send() const {}

};
