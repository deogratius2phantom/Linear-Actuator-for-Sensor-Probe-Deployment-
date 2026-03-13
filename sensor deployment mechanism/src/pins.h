#ifndef CNC_SHIELD_PINS_H
#define CNC_SHIELD_PINS_H

#include <Arduino.h>

/*
   Arduino CNC Shield V3 Pin Mapping
   Board: Arduino Uno / ATmega328P
*/

/* ----------------------------
   Stepper Driver Control Pins
   ---------------------------- */

// Global enable (active LOW)
#define CNC_ENABLE_PIN      8

// X Axis
#define X_STEP_PIN          2
#define X_DIR_PIN           5

// Y Axis
#define Y_STEP_PIN          3
#define Y_DIR_PIN           6

// Z Axis
#define Z_STEP_PIN          4
#define Z_DIR_PIN           7

// A Axis (mirrors X by default unless modified)
#define A_STEP_PIN          12
#define A_DIR_PIN           13


/* ----------------------------
   Limit Switch Inputs
   ---------------------------- */

#define X_LIMIT_PIN         9
#define Y_LIMIT_PIN         10
#define Z_LIMIT_PIN         11


/* ----------------------------
   Spindle / Coolant Control
   ---------------------------- */

#define SPINDLE_ENABLE_PIN  12
#define SPINDLE_DIR_PIN     13

#define COOLANT_PIN         A3


/* ----------------------------
   Control Inputs
   ---------------------------- */

#define ABORT_PIN           A0
#define HOLD_PIN            A1
#define RESUME_PIN          A2


/* ----------------------------
   Auxiliary
   ---------------------------- */

#define PROBE_PIN           A5


/* ----------------------------
   I2C Pins
   ---------------------------- */

#define I2C_SDA_PIN         A4
#define I2C_SCL_PIN         A5


/* ----------------------------
   Helper Macros
   ---------------------------- */

#define ENABLE_STEPPERS()   digitalWrite(CNC_ENABLE_PIN, LOW)
#define DISABLE_STEPPERS()  digitalWrite(CNC_ENABLE_PIN, HIGH)

#endif