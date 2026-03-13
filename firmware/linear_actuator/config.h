/**
 * config.h
 * Configuration parameters for the linear actuator firmware.
 *
 * Hardware: Stepper motor (NEMA 17) driven by an A4988/DRV8825 driver,
 * with two limit switches for home (retracted) and end (extended) positions.
 * Target platform: Arduino Mega 2560 / Arduino Uno.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ---------------------------------------------------------------------------
// Stepper motor driver pin mapping (Step/Dir interface)
// ---------------------------------------------------------------------------
#define STEP_PIN       3   // STEP signal to motor driver
#define DIR_PIN        4   // DIR  signal to motor driver
#define ENABLE_PIN     5   // ENABLE signal (active LOW on A4988/DRV8825)

// Microstepping resolution – must match the MS1/MS2/MS3 jumper setting on
// the driver board.  1 = full step, 2 = half, 4 = quarter, 8 = eighth,
// 16 = sixteenth step.
#define MICROSTEP_RESOLUTION  8

// Full steps per revolution of the motor (typical NEMA 17 = 200)
#define STEPS_PER_REVOLUTION  200

// Lead-screw pitch in mm per revolution (adjust to match your mechanism)
#define LEAD_SCREW_PITCH_MM   8.0f

// ---------------------------------------------------------------------------
// Motion parameters
// ---------------------------------------------------------------------------
// Maximum speed in steps per second
#define MAX_SPEED_STEPS_PER_SEC   1600

// Acceleration in steps per second squared
#define ACCELERATION_STEPS_PER_SEC2  800

// Homing speed (slower, steps per second)
#define HOMING_SPEED_STEPS_PER_SEC  400

// Total travel distance in mm (physical stroke of the actuator)
#define TOTAL_TRAVEL_MM  150.0f

// ---------------------------------------------------------------------------
// Limit switch pin mapping
// ---------------------------------------------------------------------------
// Home switch: actuator fully retracted (probe stowed)
#define LIMIT_SW_HOME_PIN  8

// End switch: actuator fully extended (probe deployed)
#define LIMIT_SW_END_PIN   9

// Wiring: switches connected between pin and GND; internal pull-up enabled.
// Switch state when triggered (LOW = normally-open switch pulled to GND).
#define LIMIT_SW_TRIGGERED_STATE  LOW

// ---------------------------------------------------------------------------
// Status LED (optional)
// ---------------------------------------------------------------------------
#define STATUS_LED_PIN  13

// ---------------------------------------------------------------------------
// Serial communication
// ---------------------------------------------------------------------------
#define SERIAL_BAUD_RATE  115200

// ---------------------------------------------------------------------------
// Derived constants (do not edit)
// ---------------------------------------------------------------------------
#define STEPS_PER_MM \
    ((float)(STEPS_PER_REVOLUTION * MICROSTEP_RESOLUTION) / LEAD_SCREW_PITCH_MM)

#define TOTAL_TRAVEL_STEPS \
    ((long)(TOTAL_TRAVEL_MM * STEPS_PER_MM))

#endif /* CONFIG_H */
