/**
 * stepper_control.cpp
 * Stepper motor control implementation using the AccelStepper library.
 *
 * Dependency: AccelStepper >= 1.64
 *   Install via Arduino Library Manager: "AccelStepper by Mike McCauley"
 */

#include "stepper_control.h"
#include "limit_switch.h"
#include <AccelStepper.h>

// AccelStepper instance configured for an external Step/Dir driver.
static AccelStepper motor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Back-off distance in steps after the home switch is triggered during homing.
static const long BACKOFF_STEPS = (long)(2.0f * STEPS_PER_MM);

// Homing timeout in milliseconds (5 seconds).
static const unsigned long HOMING_TIMEOUT_MS = 5000UL;

void stepper_init(void) {
    pinMode(ENABLE_PIN, OUTPUT);
    stepper_disable();  // start with driver disabled

    motor.setMaxSpeed((float)MAX_SPEED_STEPS_PER_SEC);
    motor.setAcceleration((float)ACCELERATION_STEPS_PER_SEC2);
    motor.setCurrentPosition(0);
}

void stepper_enable(void) {
    digitalWrite(ENABLE_PIN, LOW);   // A4988/DRV8825: active LOW
}

void stepper_disable(void) {
    digitalWrite(ENABLE_PIN, HIGH);
}

bool stepper_home(void) {
    stepper_enable();

    // Phase 1: move toward home switch at homing speed.
    motor.setMaxSpeed((float)HOMING_SPEED_STEPS_PER_SEC);
    // Move a large negative number of steps; we will stop on switch trigger.
    motor.move(-(TOTAL_TRAVEL_STEPS + BACKOFF_STEPS));

    unsigned long startMs = millis();
    while (!limitSwitch_homeTriggered()) {
        motor.run();
        if ((millis() - startMs) > HOMING_TIMEOUT_MS) {
            motor.stop();
            Serial.println(F("[ERROR] Homing timeout – home switch not found."));
            return false;
        }
    }
    motor.stop();
    motor.setCurrentPosition(0);

    // Phase 2: back off the switch slightly.
    motor.setMaxSpeed((float)HOMING_SPEED_STEPS_PER_SEC);
    motor.moveTo(BACKOFF_STEPS);
    while (motor.distanceToGo() != 0) {
        motor.run();
    }
    motor.setCurrentPosition(0);   // redefine home as 0 after back-off

    // Restore full operating speed.
    motor.setMaxSpeed((float)MAX_SPEED_STEPS_PER_SEC);

    Serial.println(F("[INFO] Homing complete. Position set to 0."));
    return true;
}

void stepper_moveTo_mm(float positionMm) {
    // Clamp to valid travel range.
    if (positionMm < 0.0f) positionMm = 0.0f;
    if (positionMm > TOTAL_TRAVEL_MM) positionMm = TOTAL_TRAVEL_MM;

    long targetSteps = (long)(positionMm * STEPS_PER_MM);
    stepper_moveTo_steps(targetSteps);
}

void stepper_moveTo_steps(long steps) {
    if (steps < 0) steps = 0;
    if (steps > TOTAL_TRAVEL_STEPS) steps = TOTAL_TRAVEL_STEPS;
    stepper_enable();
    motor.moveTo(steps);
}

void stepper_stop(void) {
    motor.stop();
}

float stepper_getPosition_mm(void) {
    return (float)motor.currentPosition() / STEPS_PER_MM;
}

long stepper_getPosition_steps(void) {
    return motor.currentPosition();
}

bool stepper_isRunning(void) {
    return motor.distanceToGo() != 0;
}

void stepper_run(void) {
    // Keep limit switch debounce state current on every loop iteration.
    limitSwitch_update();

    // Safety: stop immediately if a limit switch is triggered during motion.
    if (motor.distanceToGo() > 0 && limitSwitch_endTriggered()) {
        motor.stop();
        Serial.println(F("[WARN] End limit switch triggered – motion stopped."));
        return;
    }
    if (motor.distanceToGo() < 0 && limitSwitch_homeTriggered()) {
        motor.stop();
        Serial.println(F("[WARN] Home limit switch triggered – motion stopped."));
        return;
    }
    motor.run();
}
