#pragma once
#include <bistable.hpp>
#include <decl.hpp>

#define PRC_REPEAT_COUNT 5

template <typename TransmitterType, bool... HeaderBits>
class PlugRemoteControl : public BistableBase<true> {
public:
  PlugRemoteControl(const TransmitterType& tt) : m_transmitter{tt} {}
protected:
  void turn_on() override {
    for (unsigned int i = 0u; i < PRC_REPEAT_COUNT; i++) {
      m_transmitter.template send<HeaderBits..., PRC_BITS_ON>();
      if (i != PRC_REPEAT_COUNT-1)
        delayMicroseconds(11500);
    }
  }
  void turn_off() override {
    for (unsigned int i = 0u; i < PRC_REPEAT_COUNT; i++) {
      m_transmitter.template send<HeaderBits..., PRC_BITS_OFF>();
      if (i != PRC_REPEAT_COUNT-1)
        delayMicroseconds(11500);
    }
  }
private:
  const TransmitterType& m_transmitter;
};
