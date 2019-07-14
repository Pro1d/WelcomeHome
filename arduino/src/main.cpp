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
#include "transmitter_433.hpp"
#include "plug_remote_control.hpp"

// Global variable - declared in output_serial.hpp
OutputSerial OSerial;

using LightToggleType = LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2>;
using LightSensorType = LightSensor<ANALOG_HIGH_LUMINOSITY_PIN, ANALOG_LOW_LUMINOSITY_PIN>;
using TextAnimationType = TextAnimation<MATRIX_LED_ROWS, MATRIX_LED_COLS>;
using TransmitterType = Transmitter433<TRANSMITTER_433_PIN, PRC_PERIOD, PRC_DURATION_ONE, PRC_DURATION_ZERO>;

//DelayedDigitalOutput ddout;
LightToggleType toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;
TwinkleCore<LIGHT_DETECTION_STATUS> twinkle_core;
constexpr int MLR[MATRIX_LED_ROWS] = MATRIX_LED_ROW;
constexpr int MLC[MATRIX_LED_COLS] = MATRIX_LED_COLUMN;
Matrix<MATRIX_LED_ROWS, MATRIX_LED_COLS, MLR, MLC> matrix;
SnakeAnimation<MATRIX_LED_ROWS, MATRIX_LED_COLS> snake;
TextAnimationType text;
LightSensorType lightSensor;
AutoLightOff<LightToggleType, LightSensorType, TextAnimationType> autoLightOff(toggleLight, lightSensor, text);
Clock clock;
SensorsStream<LightSensorType> sensors_stream(lightSensor);
TransmitterType tm433;
PlugRemoteControl<TransmitterType, PRC_BITS_I1> plug_rc_1(tm433);
PlugRemoteControl<TransmitterType, PRC_BITS_I2> plug_rc_2(tm433);

bool cmd_toggle_light = false;

void on_button_pushed() {
  cmd_toggle_light = true;
}

template<int DURATION>
void blink_debug() {
  OSerial.send(C_DEBUG, NSTR("Blink built-in LED"));

  tone(TONE_PIN, 440, DURATION);
  blink<LED_BUILTIN, DURATION>();
}

void setup()
{
  randomSeed(analogRead(10));
  serial.init();
  // OSerial.init();
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
  tm433.init();
  plug_rc_1.init(false);
  plug_rc_2.init(false);
  // Push button for light
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_PIN), on_button_pushed, FALLING);

  sensors_stream.init();

  text.init();
  text.start("Welcome Home!");
}

void loop()
{
  if(serial.readNextCommand(cmd, false)) {
    bool valid_command = false;

    switch(cmd.target) {
      case DEBUG:
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
      case PRC1:
        if(cmd.action == ON) {
          plug_rc_1.on();
          valid_command = true;
        } else if(cmd.action == OFF) {
          plug_rc_1.off();
          valid_command = true;
        } else if(cmd.action == TOGGLE) {
          plug_rc_1.toggle();
          valid_command = true;
        }
        break;
      case PRC2:
        if(cmd.action == ON) {
          plug_rc_2.on();
          valid_command = true;
        } else if(cmd.action == OFF) {
          plug_rc_2.off();
          valid_command = true;
        } else if(cmd.action == TOGGLE) {
          plug_rc_2.toggle();
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
      case CLOCK:
        if(cmd.action == AUTO) {
          clock.synchronizeFromSerial();
          valid_command = true;
        }
      default:
        break;
    }

    if(!valid_command) {
      OSerial.send(C_DEBUG, NSTR("Unknown command"));
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
