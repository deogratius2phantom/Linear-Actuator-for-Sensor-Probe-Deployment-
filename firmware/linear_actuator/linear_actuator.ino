/**
 * linear_actuator.ino
 * Main Arduino sketch for the stepper-motor-driven linear actuator used to
 * deploy an NPK soil probe on a robotic platform.
 *
 * ── Behaviour ──────────────────────────────────────────────────────────────
 *  1. On power-up the actuator runs a homing sequence (retracts to the home
 *     limit switch) to establish a known reference position.
 *  2. Commands are accepted over the hardware Serial port at SERIAL_BAUD_RATE.
 *  3. The main loop continuously services the stepper motor and echoes the
 *     current position once per second.
 *
 * ── Serial Command Protocol ─────────────────────────────────────────────────
 *  All commands are terminated with a newline ('\n').
 *
 *  Command     | Description
 *  ------------|-------------------------------------------------------------
 *  H           | Run homing sequence (retract to home switch, zero position)
 *  D           | Deploy: move to fully extended position (TOTAL_TRAVEL_MM)
 *  R           | Retract: move to home position (0 mm)
 *  M<value>    | Move to absolute position in mm, e.g.  M75.5
 *  S           | Stop motor immediately
 *  P           | Print current position in mm
 *  ?           | Print this help text
 *
 * ── Dependencies ────────────────────────────────────────────────────────────
 *  AccelStepper >= 1.64  (Arduino Library Manager: "AccelStepper by Mike McCauley")
 *
 * ── Hardware ─────────────────────────────────────────────────────────────────
 *  See config.h for pin assignments and motion parameters.
 *  See hardware/pinout.md for full wiring details.
 */

#include "config.h"
#include "stepper_control.h"
#include "limit_switch.h"

// ── State ───────────────────────────────────────────────────────────────────
static String serialBuffer = "";
static unsigned long lastPrintMs = 0;

// ── Forward declarations ─────────────────────────────────────────────────────
static void processCommand(const String &cmd);
static void printHelp(void);

// ── Setup ────────────────────────────────────────────────────────────────────
void setup(void) {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) { /* wait for USB-CDC on Leonardo/Mega 2560 */ }

    pinMode(STATUS_LED_PIN, OUTPUT);
    digitalWrite(STATUS_LED_PIN, LOW);

    limitSwitch_init();
    stepper_init();

    Serial.println(F("=============================================="));
    Serial.println(F(" Linear Actuator – NPK Probe Deployment v1.0"));
    Serial.println(F("=============================================="));
    Serial.println(F("Type '?' for command help."));
    Serial.println(F("Starting homing sequence..."));

    bool homed = stepper_home();
    if (homed) {
        digitalWrite(STATUS_LED_PIN, HIGH);  // LED on = ready
    } else {
        Serial.println(F("[ERROR] Homing failed. Check limit switches and wiring."));
    }
}

// ── Loop ─────────────────────────────────────────────────────────────────────
void loop(void) {
    // Service stepper motor as fast as possible.
    stepper_run();

    // Non-blocking serial command reading.
    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == '\n' || c == '\r') {
            serialBuffer.trim();
            if (serialBuffer.length() > 0) {
                processCommand(serialBuffer);
            }
            serialBuffer = "";
        } else {
            serialBuffer += c;
        }
    }

    // Print position once per second.
    if ((millis() - lastPrintMs) >= 1000UL) {
        lastPrintMs = millis();
        Serial.print(F("Position: "));
        Serial.print(stepper_getPosition_mm(), 2);
        Serial.print(F(" mm  |  "));
        Serial.print(stepper_isRunning() ? F("MOVING") : F("IDLE"));
        Serial.println();
    }
}

// ── Command processing ────────────────────────────────────────────────────────
static void processCommand(const String &cmd) {
    char first = (char)toupper((unsigned char)cmd.charAt(0));

    switch (first) {
        case 'H':
            Serial.println(F("CMD: Homing..."));
            stepper_home();
            break;

        case 'D':
            Serial.println(F("CMD: Deploying probe (moving to full extension)..."));
            stepper_moveTo_mm(TOTAL_TRAVEL_MM);
            break;

        case 'R':
            Serial.println(F("CMD: Retracting probe (moving to home)..."));
            stepper_moveTo_mm(0.0f);
            break;

        case 'M': {
            float target = cmd.substring(1).toFloat();
            Serial.print(F("CMD: Moving to "));
            Serial.print(target, 2);
            Serial.println(F(" mm"));
            stepper_moveTo_mm(target);
            break;
        }

        case 'S':
            Serial.println(F("CMD: Stop."));
            stepper_stop();
            break;

        case 'P':
            Serial.print(F("Position: "));
            Serial.print(stepper_getPosition_mm(), 2);
            Serial.println(F(" mm"));
            break;

        case '?':
            printHelp();
            break;

        default:
            Serial.print(F("[WARN] Unknown command: "));
            Serial.println(cmd);
            break;
    }
}

static void printHelp(void) {
    Serial.println(F("--- Command Reference ---"));
    Serial.println(F("  H       - Home (retract to limit switch, zero position)"));
    Serial.println(F("  D       - Deploy (extend to full travel)"));
    Serial.println(F("  R       - Retract (return to home position)"));
    Serial.println(F("  M<val>  - Move to absolute position in mm (e.g. M75.5)"));
    Serial.println(F("  S       - Stop motor immediately"));
    Serial.println(F("  P       - Print current position"));
    Serial.println(F("  ?       - Show this help text"));
    Serial.println(F("-------------------------"));
}
