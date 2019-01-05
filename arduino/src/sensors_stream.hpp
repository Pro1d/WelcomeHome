#pragma once

#define SENSORS_STREAM_PERIOD 1000 // ms

template <typename Stream, typename LightSensorType>
class SensorsStream {
public:
  SensorsStream(Stream& stream, const LightSensorType& lsensor)
    : light_sensor{lsensor}
    , stream{stream}
  {
  }

  void init() {
    last_frame = 0;
  }

  void update() {
    const unsigned long t = millis();

    if(t - last_frame < SENSORS_STREAM_PERIOD)
      return;

    last_frame = t;
    
    stream.send(C_SENSORS,
        light_sensor.is_light_on(),
        light_sensor.is_day_light(),
        light_sensor.get_luminosity_high(),
        light_sensor.get_luminosity_low());
  }

private:
  unsigned long last_frame;
  const LightSensorType& light_sensor;
  Stream& stream;
};
