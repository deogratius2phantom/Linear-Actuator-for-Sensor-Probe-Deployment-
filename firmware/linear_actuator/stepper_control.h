/**
 * stepper_control.h
 * Stepper motor control interface for the linear actuator.
 *
 * Built on top of the AccelStepper library to provide acceleration,
 * position tracking, and limit-switch-aware movement.
 */

#ifndef STEPPER_CONTROL_H
#define STEPPER_CONTROL_H

#include <Arduino.h>
#include "config.h"

/**
 * Initialise the stepper driver pins and AccelStepper instance.
 * Call once from setup().
 */
void stepper_init(void);

/**
 * Enable the stepper motor driver (de-asserts ENABLE pin).
 */
void stepper_enable(void);

/**
 * Disable the stepper motor driver (asserts ENABLE pin).
 * Disabling removes holding torque but reduces heat and power consumption.
 */
void stepper_disable(void);

/**
 * Perform a homing sequence:
 *   1. Move toward the home limit switch until it triggers.
 *   2. Back off a few steps to release the switch.
 *   3. Set the current position as step 0.
 *
 * Blocks until homing is complete or a timeout occurs.
 *
 * @return true  if homing succeeded.
 * @return false if a timeout or unexpected switch state was detected.
 */
bool stepper_home(void);

/**
 * Move the actuator to a target position expressed in millimetres from home.
 * The function returns immediately; call stepper_run() in the main loop to
 * execute the motion.
 *
 * @param positionMm  Target position in mm (0 = home / retracted,
 *                    TOTAL_TRAVEL_MM = fully extended).
 */
void stepper_moveTo_mm(float positionMm);

/**
 * Move the actuator to a target position expressed in steps from home.
 * The function returns immediately; call stepper_run() in the main loop.
 *
 * @param steps  Target position in steps.
 */
void stepper_moveTo_steps(long steps);

/**
 * Stop the motor immediately (no deceleration ramp).
 */
void stepper_stop(void);

/**
 * Return the current position in mm from home.
 */
float stepper_getPosition_mm(void);

/**
 * Return the current position in steps from home.
 */
long stepper_getPosition_steps(void);

/**
 * Return true if the motor is still moving toward the target position.
 */
bool stepper_isRunning(void);

/**
 * Service the stepper motor – must be called as fast as possible from loop().
 * Checks limit switches and issues step pulses as required.
 */
void stepper_run(void);

#endif /* STEPPER_CONTROL_H */
