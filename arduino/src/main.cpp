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

template<int DURATION>
void blink_debug() {
  Serial.println("Blink built-in LED");

  tone(TONE_PIN, 440, DURATION);
  blink<LED_BUILTIN, DURATION>();
}

//DelayedDigitalOutput ddout;
LatchingRelayControl<LIGHT_COIL_1, LIGHT_COIL_2> toggleLight;
LatchingRelayControl<TETRIS_COIL_SET, TETRIS_COIL_RESET> tetris;
CommandSerial serial;
Command cmd;
TwinkleCore<LIGHT_DETECTION_STATUS> twinkle_core;
constexpr int MLR[5] = MATRIX_LED_ROW;
constexpr int MLC[7] = MATRIX_LED_COLUMN;
Matrix<5, 7, MLR, MLC> matrix;
SnakeAnimation<5, 7> snake;
TextAnimation<5, 7> text;

bool cmd_toggle_light = false;

void on_button_pushed() {
  cmd_toggle_light = true;
}

void print_debug() {
  int l1 = analogRead(ANALOG_HIGH_LUMINOSITY_PIN);
  int l2 = analogRead(ANALOG_LOW_LUMINOSITY_PIN);
  int t = analogRead(ANALOG_TEMPERATURE_PIN);
  Serial.print("L: ");
  Serial.print(l1, DEC);
  Serial.print(" ");
  Serial.print(l2, DEC);
  Serial.print(" T: ");
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
  // Push button for light
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PUSH_BUTTON_PIN), on_button_pushed, FALLING);

  text.init();
  text.start("LOADING: 42% - ([{<\">}]) '3+X^5=-2/PI ?! |`\\@&.,;");
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
  
  int l1 = analogRead(ANALOG_HIGH_LUMINOSITY_PIN);
  int l2 = analogRead(ANALOG_LOW_LUMINOSITY_PIN);
  twinkle_core.setMode(l1-l2 > 100 ? BLAZING : UNSTABLE);

  twinkle_core.update();

  text.update(matrix);
  if(!text.is_playing())
    snake.update(matrix);
  matrix.update();
}
