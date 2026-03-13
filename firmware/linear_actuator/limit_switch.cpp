/**
 * limit_switch.cpp
 * Limit switch implementation with non-blocking millis()-based debouncing.
 *
 * Each switch tracks its raw pin state and the timestamp of the last change.
 * The stable (debounced) state is updated only when the raw reading has been
 * consistent for at least DEBOUNCE_DELAY_MS milliseconds, so the stepper
 * motor can continue to be serviced without any blocking delay().
 */

#include "limit_switch.h"

// Per-switch debounce state
struct SwitchState {
    uint8_t           pin;
    bool              stableTriggered; // last confirmed debounced state
    int               lastRaw;         // last raw digitalRead value
    unsigned long     lastChangeMs;    // millis() when raw state last changed
};

static SwitchState s_home;
static SwitchState s_end;

/**
 * Update a single switch's debounce state machine.
 * Must be called frequently (e.g. from stepper_run / loop) for accurate results.
 */
static void updateSwitch(SwitchState &sw) {
    int raw = digitalRead(sw.pin);
    if (raw != sw.lastRaw) {
        sw.lastRaw      = raw;
        sw.lastChangeMs = millis();
    }
    if ((millis() - sw.lastChangeMs) >= DEBOUNCE_DELAY_MS) {
        sw.stableTriggered = (raw == LIMIT_SW_TRIGGERED_STATE);
    }
}

void limitSwitch_init(void) {
    pinMode(LIMIT_SW_HOME_PIN, INPUT_PULLUP);
    pinMode(LIMIT_SW_END_PIN,  INPUT_PULLUP);

    s_home.pin              = LIMIT_SW_HOME_PIN;
    s_home.lastRaw          = digitalRead(LIMIT_SW_HOME_PIN);
    s_home.stableTriggered  = (s_home.lastRaw == LIMIT_SW_TRIGGERED_STATE);
    s_home.lastChangeMs     = millis();

    s_end.pin               = LIMIT_SW_END_PIN;
    s_end.lastRaw           = digitalRead(LIMIT_SW_END_PIN);
    s_end.stableTriggered   = (s_end.lastRaw == LIMIT_SW_TRIGGERED_STATE);
    s_end.lastChangeMs      = millis();
}

void limitSwitch_update(void) {
    updateSwitch(s_home);
    updateSwitch(s_end);
}

bool limitSwitch_homeTriggered(void) {
    updateSwitch(s_home);
    return s_home.stableTriggered;
}

bool limitSwitch_endTriggered(void) {
    updateSwitch(s_end);
    return s_end.stableTriggered;
}
