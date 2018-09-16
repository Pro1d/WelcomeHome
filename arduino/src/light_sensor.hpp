#pragma once

#define LIGHT_SENSOR_PERIOD       1000
#define LIGHT_ON_DIFF_THRESHOLD   100
#define DAY_LIGHT_LIGHT_ON_LUMINOSITY_THRESHOLD   100
#define DAY_LIGHT_LIGHT_OFF_LUMINOSITY_THRESHOLD  50

template <int HIGH_LUMINOSITY_PIN, int LOW_LUMINOSITY_PIN>
class LightSensor {
public:
  void init() {
    day_light_threshold[false] = DAY_LIGHT_LIGHT_OFF_LUMINOSITY_THRESHOLD;
    day_light_threshold[true] = DAY_LIGHT_LIGHT_ON_LUMINOSITY_THRESHOLD;
  }

  void update() {
    const unsigned long t = millis();
    
    if(t - last_frame < LIGHT_SENSOR_PERIOD)
      return;
    
    last_frame = t;

    luminosity_high = analogRead(HIGH_LUMINOSITY_PIN);
    luminosity_low = analogRead(LOW_LUMINOSITY_PIN);
    light_on = luminosity_high - luminosity_low > LIGHT_ON_DIFF_THRESHOLD;
    day_light = luminosity_low > day_light_threshold[light_on];
  }

  void bad_threshold() {
    Serial.print("Luminosity threshold when ceil light is ");
    Serial.print(light_on, DEC);
    Serial.print(" is bad. Change it from ");
    Serial.print(day_light_threshold[light_on], DEC);
    Serial.print(" to ");
    day_light_threshold[light_on] = luminosity_low + (day_light * 2 - 1);
    Serial.print(day_light_threshold[light_on], DEC);
    Serial.println(".");
  }

  int get_luminosity_high() {
    return luminosity_high;
  }

  int get_luminosity_low() {
    return luminosity_low;
  }

  bool is_light_on() {
    return light_on;
  }

  bool is_day_light() {
    return day_light;
  }

private:
  unsigned long last_frame;
  int day_light_threshold[2];
  int luminosity_high;
  int luminosity_low;
  bool light_on;
  bool day_light;
};
