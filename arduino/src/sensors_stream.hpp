#pragma once

#define SENSORS_STREAM_PERIOD 60000uL // ms

template <typename LightSensorType, typename TemperatureSensorType>
class SensorsStream {
public:
  SensorsStream(const LightSensorType& lsensor, const TemperatureSensorType& tsensor)
    : light_sensor{lsensor}, temperature_sensor{tsensor}
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
    
    OSerial.send(C_SENSORS,
        light_sensor.is_light_on(),
        light_sensor.is_day_light(),
        light_sensor.get_luminosity_high(),
        light_sensor.get_luminosity_low(),
        temperature_sensor.getTemperatureExt(),
        temperature_sensor.getTemperatureInt());
  }

private:
  unsigned long last_frame;
  const LightSensorType& light_sensor;
  const TemperatureSensorType& temperature_sensor;
};
