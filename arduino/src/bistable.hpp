#pragma once


template <bool ForceMode>
class BistableBase {
public:
  void init(bool initial_state) {
    is_on = initial_state;
    if(ForceMode) {
      if(is_on)
        turn_on();
      else
        turn_off();
    }
  }

  void on() {
    if(ForceMode or not is_on)
      turn_on();
    is_on = true;
  }

  void off() {
    if(ForceMode or is_on)
      turn_off();
    is_on = false;
  }

  void toggle() {
    is_on = not is_on;
    if(is_on)
      turn_on();
    else
      turn_off();
  }
protected:
  virtual void turn_on() = 0;
  virtual void turn_off() = 0;
private:
  bool is_on;
};
