#include <Arduino.h>
#include "pins.h"
#include <PinChangeInterrupt.h>

// -------------------------------------------------------
// Lead screw configuration (like a 3D printer Z axis)
// T8*8: 8mm travel per full revolution
// Steps/mm = (STEPS_PER_REV * MICROSTEPPING) / LEAD_PITCH
// -------------------------------------------------------
#define STEPS_PER_REV    200   // 0.9deg stepper = 400, 1.8deg = 200
#define MICROSTEPPING    16    // DRV8825 microstepping
#define LEAD_SCREW_PITCH 8     // mm per revolution (T8*8)
#define STEPS_PER_MM     ((STEPS_PER_REV * MICROSTEPPING) / LEAD_SCREW_PITCH) // 800 steps/mm

#define BACKOFF_DISTANCE_MM  2.0f   // Back-off after homing (mm)
#define DEPLOY_DISTANCE_MM   154.0f // Default deploy distance (mm)
#define MAX_TRAVEL_MM        155.0f // Hard limit - maximum travel from home (mm)

#define HOMING_SPEED_HZ  16000
#define BACKOFF_SPEED_HZ 800
#define DEPLOY_SPEED_HZ  16000
#define MIN_SPEED_HZ     1000    // Start/end speed for accel & decel ramp
#define ACCEL_DISTANCE_MM 15.0f // Distance (mm) over which to ramp up/down

#define HOMING_DIR 0  // Towards limit switch (zero point)
#define DEPLOY_DIR 1  // Away from limit switch (positive)

// State machine
enum SystemState {
  IDLE,
  HOMING,
  BACKING_OFF,
  HOLDING,
  DEPLOYING
};

volatile SystemState currentState = IDLE;
float currentPositionMM = 0.0f; // Tracks position from home (mm)
String serialCommand;

// Constant-speed move — used for homing back-off only
void moveMM(float distanceMM, int speedHz, int direction) {
  long steps = (long)(distanceMM * STEPS_PER_MM);
  float stepSizeMM = 1.0f / STEPS_PER_MM;
  digitalWrite(Z_DIR_PIN, direction);
  long pulseDelay = 1000000L / (speedHz * 2);
  for (long i = 0; i < steps; i++) {
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(pulseDelay);
    currentPositionMM += (direction == DEPLOY_DIR) ? stepSizeMM : -stepSizeMM;
  }
}

// Trapezoidal accel/decel move — used for all deployment moves
// Profile: ramp up (MIN -> max) | cruise | ramp down (max -> MIN)
void moveMM_accel(float distanceMM, int maxSpeedHz, int direction) {
  long totalSteps = (long)(distanceMM * STEPS_PER_MM);
  if (totalSteps == 0) return;

  // Ramp steps; auto-shorten to triangle profile for short moves
  long accelSteps = (long)(ACCEL_DISTANCE_MM * STEPS_PER_MM);
  if (accelSteps > totalSteps / 1.5) accelSteps = totalSteps / 1.5;

  float stepSizeMM = 1.0f / STEPS_PER_MM;
  digitalWrite(Z_DIR_PIN, direction);

  for (long i = 0; i < totalSteps; i++) {
    float currentHz;
    if (i < accelSteps) {
      // Ramp up
      currentHz = MIN_SPEED_HZ + (float)(maxSpeedHz - MIN_SPEED_HZ) * i / accelSteps;
    } else if (i >= totalSteps - accelSteps) {
      // Ramp down
      long decelStep = totalSteps - i - 1;
      currentHz = MIN_SPEED_HZ + (float)(maxSpeedHz - MIN_SPEED_HZ) * decelStep / accelSteps;
    } else {
      // Cruise at max speed
      currentHz = maxSpeedHz;
    }
    long pulseDelay = 1000000L / (long)(currentHz * 2);
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(pulseDelay);
    currentPositionMM += (direction == DEPLOY_DIR) ? stepSizeMM : -stepSizeMM;
  }
}

void moveUntilLimit(int speedHz, int direction) {
  digitalWrite(Z_DIR_PIN, direction);
  long pulseDelay = 1000000 / (speedHz * 2);
  
  // Move until limit switch is triggered
  while (digitalRead(Z_LIMIT_PIN) == HIGH) {
    digitalWrite(Z_STEP_PIN, HIGH);
    delayMicroseconds(pulseDelay);
    digitalWrite(Z_STEP_PIN, LOW);
    delayMicroseconds(pulseDelay);
  }
}

void performHoming() {
  Serial.println("Starting homing sequence...");
  currentState = HOMING;
  ENABLE_STEPPERS();

  // Move towards limit switch until triggered
  moveUntilLimit(HOMING_SPEED_HZ, HOMING_DIR);
  currentPositionMM = 0.0f; // Zero position at the switch

  Serial.print("Limit switch hit. Backing off ");
  Serial.print(BACKOFF_DISTANCE_MM);
  Serial.println("mm...");
  moveMM(BACKOFF_DISTANCE_MM, BACKOFF_SPEED_HZ, DEPLOY_DIR);

  Serial.print("Homing complete. Position: ");
  Serial.print(currentPositionMM);
  Serial.println("mm. Ready for commands.");
  currentState = HOLDING;
}

void deploySensor(float distanceMM) {
  float target = currentPositionMM + distanceMM;
  if (target > MAX_TRAVEL_MM) {
    distanceMM = MAX_TRAVEL_MM - currentPositionMM;
    Serial.print("Clamped to max travel. Deploying ");
    Serial.print(distanceMM);
    Serial.println("mm...");
  } else {
    Serial.print("Deploying sensor ");
    Serial.print(distanceMM);
    Serial.println("mm...");
  }
  currentState = DEPLOYING;
  moveMM_accel(distanceMM, DEPLOY_SPEED_HZ, DEPLOY_DIR);
  Serial.print("Deployment complete. Position: ");
  Serial.print(currentPositionMM);
  Serial.println("mm.");
  currentState = HOLDING;
}

// Move relative to current position (+mm = deploy direction, -mm = home direction)
void moveRelative(float distanceMM) {
  float target = currentPositionMM + distanceMM;
  if (target > MAX_TRAVEL_MM) {
    distanceMM = MAX_TRAVEL_MM - currentPositionMM;
    Serial.print("Clamped to max travel (150mm). Moving ");
    Serial.print(distanceMM);
    Serial.println("mm instead...");
  } else if (target < 0) {
    distanceMM = -currentPositionMM;
    Serial.print("Clamped to home (0mm). Moving ");
    Serial.print(distanceMM);
    Serial.println("mm instead...");
  } else {
    Serial.print("Moving ");
    Serial.print(distanceMM);
    Serial.println("mm...");
  }
  int dir = (distanceMM >= 0) ? DEPLOY_DIR : HOMING_DIR;
  moveMM_accel(abs(distanceMM), DEPLOY_SPEED_HZ, dir);
  Serial.print("Move complete. Position: ");
  Serial.print(currentPositionMM);
  Serial.println("mm.");
}

// Move to an absolute position from home (mm)
void moveAbsolute(float targetMM) {
  if (targetMM > MAX_TRAVEL_MM) {
    Serial.print("Error: target ");
    Serial.print(targetMM);
    Serial.print("mm exceeds max travel of ");
    Serial.print(MAX_TRAVEL_MM);
    Serial.println("mm.");
    return;
  }
  float delta = targetMM - currentPositionMM;
  if (abs(delta) < (1.0f / STEPS_PER_MM)) {
    Serial.print("Already at ");
    Serial.print(currentPositionMM);
    Serial.println("mm.");
    return;
  }
  int dir = (delta > 0) ? DEPLOY_DIR : HOMING_DIR;
  Serial.print("Moving to ");
  Serial.print(targetMM);
  Serial.print("mm (delta: ");
  Serial.print(delta);
  Serial.println("mm)...");
  moveMM_accel(abs(delta), DEPLOY_SPEED_HZ, dir);
  Serial.print("Move complete. Position: ");
  Serial.print(currentPositionMM);
  Serial.println("mm.");
}

void printHelp() {
  Serial.println();
  Serial.println("============================================");
  Serial.println("   Sensor Deployment System - Command Help  ");
  Serial.println("============================================");
  Serial.println("  home              - Home to limit switch");
  Serial.println("  deploy <mm>       - Deploy a set distance");
  Serial.println("                      e.g. 'deploy 100'");
  Serial.println("  move <mm>         - Move relative distance");
  Serial.println("                      (+ = away, - = home)");
  Serial.println("                      e.g. 'move -20'");
  Serial.println("  goto <mm>         - Move to absolute position");
  Serial.println("                      from home e.g. 'goto 80'");
  Serial.println("  withdraw          - Return to home position");
  Serial.println("  status            - Show state & position");
  Serial.println("  help              - Show this help menu");
  Serial.println("============================================");
  Serial.print  ("  Steps/mm: ");
  Serial.println(STEPS_PER_MM);
  Serial.println("============================================");
  Serial.println();
}

void printStatus() {
  Serial.print("State   : ");
  switch (currentState) {
    case IDLE:        Serial.println("IDLE - Send 'home' to start."); break;
    case HOMING:      Serial.println("HOMING"); break;
    case BACKING_OFF: Serial.println("BACKING OFF"); break;
    case HOLDING:     Serial.println("HOLDING - Awaiting commands."); break;
    case DEPLOYING:   Serial.println("DEPLOYING"); break;
  }
  Serial.print("Position: ");
  Serial.print(currentPositionMM);
  Serial.println(" mm");
  Serial.print("Steps/mm: ");
  Serial.println(STEPS_PER_MM);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(Z_STEP_PIN, OUTPUT);
  pinMode(Z_DIR_PIN, OUTPUT);
  pinMode(CNC_ENABLE_PIN, OUTPUT);
  DISABLE_STEPPERS();

  pinMode(Z_LIMIT_PIN, INPUT_PULLUP);

  serialCommand.reserve(32);
  printHelp();
}

void processSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      serialCommand.trim();

      if (serialCommand.equalsIgnoreCase("help")) {
        printHelp();

      } else if (serialCommand.equalsIgnoreCase("status")) {
        printStatus();

      } else if ((serialCommand.equalsIgnoreCase("home") || serialCommand.equalsIgnoreCase("withdraw"))
          && (currentState == IDLE || currentState == HOLDING)) {
        performHoming();

      } else if (serialCommand.startsWith("deploy") && currentState == HOLDING) {
        // Accept "deploy" (default) or "deploy <mm>"
        float dist = DEPLOY_DISTANCE_MM;
        if (serialCommand.length() > 7) {
          dist = serialCommand.substring(7).toFloat();
        }
        if (dist > 0) {
          deploySensor(dist);
        } else {
          Serial.println("Invalid distance. Usage: deploy <mm>  e.g. 'deploy 100'");
        }

      } else if (serialCommand.startsWith("move") && currentState == HOLDING) {
        // Accept "move <mm>" (+mm = deploy direction, -mm = home direction)
        if (serialCommand.length() > 5) {
          float dist = serialCommand.substring(5).toFloat();
          moveRelative(dist);
        } else {
          Serial.println("Usage: move <mm>  e.g. 'move 20' or 'move -10'");
        }

      } else if (serialCommand.startsWith("goto") && currentState == HOLDING) {
        // Move to an absolute position from home
        if (serialCommand.length() > 5) {
          float target = serialCommand.substring(5).toFloat();
          if (target >= 0) {
            moveAbsolute(target);
          } else {
            Serial.println("Position must be >= 0mm. Usage: goto <mm>  e.g. 'goto 80'");
          }
        } else {
          Serial.println("Usage: goto <mm>  e.g. 'goto 80'");
        }

      } else if (serialCommand.length() > 0) {
        Serial.println("Unknown command or wrong state. Send 'help' for commands.");
      }

      serialCommand = "";
    } else {
      serialCommand += c;
    }
  }
}

void loop() {
  processSerial();
}