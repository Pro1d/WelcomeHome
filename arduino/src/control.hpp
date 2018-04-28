#pragma once


template<int PIN, int DURATION>
void blink() {
  digitalWrite(PIN, HIGH);
  delay(DURATION);
  digitalWrite(PIN, LOW);
}
