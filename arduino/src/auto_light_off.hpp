#pragma once

#define AUTO_LIGHT_OFF_PERIOD (5 * 1000)

template <typename LightToggleT, typename LightSensorT, typename TextAnimationT>
class AutoLightOff {
public:
  AutoLightOff(LightToggleT& toggle, LightSensorT& sensor, TextAnimationT& text) :
    lightToggle(toggle), lightSensor(sensor), textAnim(text)
  {}

  void init()
  {
    last_frame = 0;
  }

  void update()
  {
    const unsigned long t = millis();

    if(t - last_frame < AUTO_LIGHT_OFF_PERIOD)
      return;

    last_frame = t;

    if(lightSensor.is_light_on() && lightSensor.is_day_light()) {
      lightToggle.toggle();
      textAnim.start("~ Auto Light Off ~");
      OSerial.send(C_EVENT, E_AUTO_LIGHT_OFF, static_cast<unsigned long>(0));
    }
  }

private:
  unsigned long last_frame;
  LightToggleT& lightToggle;
  LightSensorT& lightSensor;
  TextAnimationT& textAnim;
};
