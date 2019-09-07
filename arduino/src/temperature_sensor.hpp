#pragma once

#define TEMPERATURE_SENSOR_PERIOD  (30uL * 1000uL)
// #define KY013_RESISTOR       10000 // Ohms
#define KY013_LOG_RESISTOR      9.21034037f // log(KY013_RESISTOR)
#define KY013_C1                0.001129148f
#define KY013_C2                0.000234125f
#define KY013_C3                0.0000000876741f
#define ABSOLUTE_ZERO_CELSIUS   -273.15f // °C
#define ANALOG_READ_MAX         1024.f

template <int INT_PIN, int EXT_PIN>
class TemperatureSensor {
public:
  void init() {
    last_frame = 0;
  }

  bool update() {
    const unsigned long t = millis();

    if(t - last_frame < TEMPERATURE_SENSOR_PERIOD)
      return false;

    last_frame = t;

    const auto analog_int = analogRead(INT_PIN);
    const auto analog_ext = analogRead(EXT_PIN);
    temperature_int = toDegreeCelsius(analog_int);
    temperature_ext = toDegreeCelsius(analog_ext);
    
    return true;
  }

  float getTemperatureInt() const {
    return temperature_int;
  }

  float getTemperatureExt() const {
    return temperature_ext;
  }
  
  void getTemperatures(char *txt) const {
    int i = abs(round(temperature_int));
    int e = abs(round(temperature_ext));
    txt[0] = "-+"[temperature_ext >= 0.f];
    txt[1] = (e / 10) + '0';
    txt[2] = (e % 10) + '0';
    txt[3] = '|';
    txt[4] = "-+"[temperature_int >= 0.f];
    txt[5] = (i / 10) + '0';
    txt[6] = (i % 10) + '0';
    txt[7] = '\x1f'; // °
    txt[8] = 'C';
    txt[9] = '\0';
  }

private:

  // KY-013 conversion from analog measure to °C
  float toDegreeCelsius(int analog) {
    // R2 = R1 * (1024 / analog - 1)
    // logR2 = R2
    // Kelvin = (1 / (C1 + C2 * logR2 + c3 * logR2 * logR2 * logR2)
    // T = Kelvin - 273.15
    const auto logR = log(ANALOG_READ_MAX / analog - 1.f) + KY013_LOG_RESISTOR;
    const auto kelvin = 1.f / (KY013_C1 + logR * (KY013_C2 + logR * logR * KY013_C3));
    const auto celsius = kelvin + ABSOLUTE_ZERO_CELSIUS; 
    return celsius;
  }

  unsigned long last_frame;
  float temperature_int;
  float temperature_ext;
};
