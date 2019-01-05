/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "latching_relay_control.hpp"
#include "command_serial.hpp"
#include "output_serial.hpp"
#include "control.hpp"
#include "decl.hpp"
#include "twinkle_core.hpp"
#include "matrix.hpp"
#include "snake.hpp"
#include "text_animation.hpp"
#include "light_sensor.hpp"
#include "auto_light_off.hpp"
#include "clock.hpp"
#include "sensors_stream.hpp"

#define NSTR(X) sizeof(X)-1, X

using LightToggleType = LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2>;
using LightSensorType = LightSensor<ANALOG_HIGH_LUMINOSITY_PIN, ANALOG_LOW_LUMINOSITY_PIN>;
using TextAnimationType = TextAnimation<MATRIX_LED_ROWS, MATRIX_LED_COLS>;

//DelayedDigitalOutput ddout;
LightToggleType toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;
OutputSerial output_serial;
TwinkleCore<LIGHT_DETECTION_STATUS> twinkle_core;
constexpr int MLR[MATRIX_LED_ROWS] = MATRIX_LED_ROW;
constexpr int MLC[MATRIX_LED_COLS] = MATRIX_LED_COLUMN;
Matrix<MATRIX_LED_ROWS, MATRIX_LED_COLS, MLR, MLC> matrix;
SnakeAnimation<MATRIX_LED_ROWS, MATRIX_LED_COLS> snake;
TextAnimationType text;
LightSensorType lightSensor;
AutoLightOff<LightToggleType, LightSensorType, TextAnimationType> autoLightOff(toggleLight, lightSensor, text);
Clock clock;
SensorsStream<OutputSerial, LightSensorType> sensors_stream(output_serial, lightSensor);

bool cmd_toggle_light = false;

void on_button_pushed() {
  cmd_toggle_light = true;
}

void on_bad_threshold() {
  lightSensor.bad_threshold();
}

template<int DURATION>
void blink_debug() {
  output_serial.send(C_DEBUG, NSTR("Blink built-in LED"));

  tone(TONE_PIN, 440, DURATION);
  blink<LED_BUILTIN, DURATION>();
}

void print_debug() {
  //int t = analogRead(ANALOG_TEMPERATURE_PIN);
  //Serial.print("LightHigh: ");
  //Serial.print(lightSensor.get_luminosity_high(), DEC);
  //Serial.print(" LightLow: ");
  //Serial.print(lightSensor.get_luminosity_low(), DEC);
  //Serial.print(" LightOn: ");
  //Serial.print(lightSensor.is_light_on(), DEC);
  //Serial.print(" DayLight: ");
  //Serial.print(lightSensor.is_day_light(), DEC);
  //Serial.print(" LumThres: ");
  //Serial.print(lightSensor.threshold(), DEC);
  //Serial.print(" Temperature: ");
  //Serial.println(t, DEC);
}

void setup()
{
  randomSeed(analogRead(10));
  serial.init();
  // output_serial.init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
  twinkle_core.init();
  matrix.init();
  snake.init();
  lightSensor.init();
  autoLightOff.init();
  clock.init();
  // Push button for light
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAD_THRESHOLD_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_PIN), on_button_pushed, FALLING);
  attachInterrupt(digitalPinToInterrupt(BAD_THRESHOLD_BUTTON_PIN), on_bad_threshold, FALLING);

  sensors_stream.init();

  text.init();
  text.start("Welcome Home!");
}

void loop()
{
  if(serial.readNextCommand(cmd, false)) {
    bool valid_command = false;

    switch(cmd.target) {
      case LED:
        if(cmd.action == TRIGGER) {
          blink_debug<500>();
          valid_command = true;
        }
        break;
      case LIGHT:
        if(cmd.action == TOGGLE) {
          toggleLight.toggle();
          valid_command = true;
        }
        break;
      case TETRIS:
        if(cmd.action == ON) {
          tetris.on();
          valid_command = true;
        } else if(cmd.action == OFF) {
          tetris.off();
          valid_command = true;
        } else if(cmd.action == TOGGLE) {
          tetris.toggle();
          valid_command = true;
        }
        break;
      case DEBUG:
        print_debug();
        valid_command = true;
        break;
      case CLOCK:
        if(cmd.action == AUTO) {
          clock.synchronizeFromSerial();
          valid_command = true;
        }
      default:
        break;
    }

    if(!valid_command) {
      output_serial.send(C_DEBUG, NSTR("Unknown command"));
    }
  }

  if(cmd_toggle_light) {
    cmd_toggle_light = false;
    toggleLight.toggle();
  }

  lightSensor.update();
  autoLightOff.update();
  sensors_stream.update();
  
  twinkle_core.setMode(lightSensor.is_light_on() == lightSensor.is_day_light() ? UNSTABLE : BLAZING);
  twinkle_core.update();

  if(text.is_playing()) {
    text.update(matrix);
  }
  else if(clock.update()) {
    char time[6];
    clock.getTimeF(time);
    text.start(time);
  }
  else {
    snake.update(matrix);
    if(!snake.is_playing()) {
      String str = "Score:";
      str += snake.get_score();
      str += " (best ";
      str += snake.get_best_score();
      str += ")";
      text.start(str);
      snake.reset();
    }
  }
  matrix.update();
}
