/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include "latching_relay_control.hpp"
#include "command_serial.hpp"
#include "control.hpp"
#include "decl.hpp"
#include "twinkle_core.hpp"
#include "matrix.hpp"
#include "snake.hpp"
#include "text_animation.hpp"
#include "light_sensor.hpp"
#include "auto_light_off.hpp"

template<int DURATION>
void blink_debug() {
  Serial.println("Blink built-in LED");

  tone(TONE_PIN, 440, DURATION);
  blink<LED_BUILTIN, DURATION>();
}

using LightToggleType = LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2>;
using LightSensorType = LightSensor<ANALOG_HIGH_LUMINOSITY_PIN, ANALOG_LOW_LUMINOSITY_PIN>;

//DelayedDigitalOutput ddout;
LightToggleType toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;
TwinkleCore<LIGHT_DETECTION_STATUS> twinkle_core;
constexpr int MLR[5] = MATRIX_LED_ROW;
constexpr int MLC[7] = MATRIX_LED_COLUMN;
Matrix<5, 7, MLR, MLC> matrix;
SnakeAnimation<5, 7> snake;
TextAnimation<5, 7> text;
LightSensorType lightSensor;
AutoLightOff<LightToggleType, LightSensorType> autoLightOff(toggleLight, lightSensor);

bool cmd_toggle_light = false;

void on_button_pushed() {
  cmd_toggle_light = true;
}

void on_bad_threshold() {
  lightSensor.bad_threshold();
}

void print_debug() {
  int t = analogRead(ANALOG_TEMPERATURE_PIN);
  Serial.print("LightHigh: ");
  Serial.print(lightSensor.get_luminosity_high(), DEC);
  Serial.print(" LightLow: ");
  Serial.print(lightSensor.get_luminosity_low(), DEC);
  Serial.print(" LightOn: ");
  Serial.print(lightSensor.is_light_on(), DEC);
  Serial.print(" DayLight: ");
  Serial.print(lightSensor.is_day_light(), DEC);
  Serial.print(" LumThres: ");
  Serial.print(lightSensor.threshold(), DEC);
  Serial.print(" Temperature: ");
  Serial.println(t, DEC);
}

void setup()
{
  randomSeed(analogRead(10));
  serial.init();
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  toggleLight.init(false);
  tetris.init(true);
  twinkle_core.init();
  matrix.init();
  snake.init();
  lightSensor.init();
  autoLightOff.init();
  // Push button for light
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BAD_THRESHOLD_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_PIN), on_button_pushed, FALLING);
  attachInterrupt(digitalPinToInterrupt(BAD_THRESHOLD_BUTTON_PIN), on_bad_threshold, FALLING);

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
      default:
        break;
    }

    if(!valid_command) {
      Serial.print("Unknown command: (");
      Serial.write(cmd.target);
      Serial.print(", ");
      Serial.write(cmd.action);
      Serial.println(")");
    }
  }

  if(cmd_toggle_light) {
    cmd_toggle_light = false;
    toggleLight.toggle();
  }

  lightSensor.update();
  autoLightOff.update();
  
  twinkle_core.setMode(lightSensor.is_light_on() == lightSensor.is_day_light() ? UNSTABLE : BLAZING);
  twinkle_core.update();

  if(text.is_playing()) {
    text.update(matrix);
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
