#pragma once

#define AUTO_LIGHT_OFF_PERIOD (5 * 1000)

template <typename LightToggleT, typename LightSensorT>
class AutoLightOff {
public:
  AutoLightOff(LightToggleT& toggle, LightSensorT& sensor) :
    lightToggle(toggle), lightSensor(sensor)
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

    if(lightSensor.is_light_on() && lightSensor.is_day_light())
      lightToggle.toggle();
  }

private:
  unsigned long last_frame;
  LightToggleT& lightToggle;
  LightSensorT& lightSensor;
};
