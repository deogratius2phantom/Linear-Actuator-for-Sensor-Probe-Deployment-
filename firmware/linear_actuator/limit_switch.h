/**
 * limit_switch.h
 * Limit switch interface for the linear actuator.
 *
 * Provides non-blocking, millis()-based debounced reading of the home and
 * end limit switches.  Call limitSwitch_update() as frequently as possible
 * from the main loop (or from stepper_run()) to keep the debounce state
 * current without blocking the stepper motor service routine.
 */

#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include <Arduino.h>
#include "config.h"

// Debounce period in milliseconds
#define DEBOUNCE_DELAY_MS  20

/**
 * Initialise both limit switch pins with internal pull-up resistors.
 * Call once from setup().
 */
void limitSwitch_init(void);

/**
 * Update the debounce state machines for both switches.
 * Call as frequently as possible from loop() or stepper_run() to ensure
 * accurate readings without blocking.
 */
void limitSwitch_update(void);

/**
 * Return true if the home (retracted) limit switch is currently triggered
 * (stable debounced state).
 */
bool limitSwitch_homeTriggered(void);

/**
 * Return true if the end (extended) limit switch is currently triggered
 * (stable debounced state).
 */
bool limitSwitch_endTriggered(void);

#endif /* LIMIT_SWITCH_H */
