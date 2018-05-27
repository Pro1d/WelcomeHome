#pragma once

#include "Arduino.h"

class DigitalOutputEvent {
  bool operator>(const DigitalOutputEvent& e) {
    return date > e.date;
  }
  unsigned long date;
  int pin, value;
};

template<typename Event>
class DelayedEvent {
  public:
    void push(const Event& e) {
      queue.push(e);
    }

    void update() {
      unsigned long date = millis(); 
      // date is reset every 50 days (2^32 millisecond)
      // -> invalid comparison 
      while(queue.top().date < date) {
        execute(queue.top());
        queue.pop();
      }
    }
    
  private:
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> queue;
};

class DelayedDigitalOuput : public DelayedEvent<DigitalOutputEvent> {
  protected:
    void execute(const DigitalOutputEvent& e) {
      digitalWrite(e.pin, e.value);
    }
};
