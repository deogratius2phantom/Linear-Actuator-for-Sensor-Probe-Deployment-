# Linear Actuator for Sensor Probe Deployment

Firmware and hardware repository for a stepper-motor-driven linear actuator used to deploy an NPK soil probe on a robotic platform.

## Overview

The system extends and retracts an NPK probe vertically using a NEMA 17 stepper motor coupled to a T8 lead screw. Two micro-switch limit switches protect against over-travel and provide a reference home position. An Arduino Mega 2560 (or Uno) runs the firmware and accepts simple serial commands for deployment control.

```
 ┌─────────────────────────────────────────────────────────┐
 │  Robot Platform                                          │
 │                                                          │
 │  ┌──────────────┐    Step/Dir     ┌──────────────────┐  │
 │  │ Arduino Mega │ ─────────────►  │  A4988 Driver    │  │
 │  │  2560        │                 │                  │  │
 │  │              │ ◄── LS1 (Home)  └──────┬───────────┘  │
 │  │              │ ◄── LS2 (End)          │ (coils)       │
 │  └──────────────┘                 ┌──────▼───────────┐  │
 │                                   │  NEMA 17 Motor   │  │
 │                                   └──────┬───────────┘  │
 │                                          │ lead screw    │
 │                                   ┌──────▼───────────┐  │
 │                                   │  Probe Carriage  │  │
 │                                   │  + NPK Sensor    │  │
 │                                   └──────────────────┘  │
 └─────────────────────────────────────────────────────────┘
```

## Repository Structure

```
├── firmware/
│   └── linear_actuator/
│       ├── linear_actuator.ino   # Main Arduino sketch
│       ├── config.h              # Pin assignments and motion parameters
│       ├── stepper_control.h     # Stepper control API
│       ├── stepper_control.cpp   # Stepper control implementation (AccelStepper)
│       ├── limit_switch.h        # Limit switch API
│       └── limit_switch.cpp      # Debounced limit switch implementation
└── hardware/
    ├── README.md                 # Hardware overview and safety notes
    ├── BOM.md                    # Bill of Materials
    ├── pinout.md                 # Wiring and pin-out reference
    └── schematics/               # KiCad schematic and PCB files
        └── README.md
```

## Hardware Requirements

| Component | Specification |
|-----------|---------------|
| Microcontroller | Arduino Mega 2560 (or Uno) |
| Stepper Motor | NEMA 17, 1.8°/step, 12 V, 1.5 A (e.g. 17HS4401) |
| Motor Driver | A4988 or DRV8825 breakout board |
| Limit Switches | Micro switch SPDT, normally-open (×2) |
| Power Supply | 12 V DC, ≥ 2 A |
| Linear Mechanism | T8 lead screw (8 mm pitch), MGN12 guide rail |

See [`hardware/BOM.md`](hardware/BOM.md) for the full component list and [`hardware/pinout.md`](hardware/pinout.md) for wiring details.

## Firmware Dependencies

Install via the **Arduino Library Manager** before compiling:

| Library | Version | Author |
|---------|---------|--------|
| AccelStepper | ≥ 1.64 | Mike McCauley |

## Quick Start

1. **Wire the hardware** – follow [`hardware/pinout.md`](hardware/pinout.md).
2. **Install dependency** – open Arduino IDE → *Tools → Manage Libraries* → search for **AccelStepper** and install.
3. **Configure** – edit `firmware/linear_actuator/config.h` to match your pin assignments, motor specifications, and travel distance.
4. **Upload** – open `firmware/linear_actuator/linear_actuator.ino` in the Arduino IDE, select your board and port, then click *Upload*.
5. **Home the actuator** – open the Serial Monitor at **115200 baud** and send `H` to run the homing sequence.
6. **Deploy the probe** – send `D` to extend to full travel, or `M<value>` for a specific position in mm.

## Serial Command Reference

| Command | Description |
|---------|-------------|
| `H` | Home – retract to limit switch and zero position |
| `D` | Deploy – extend to full travel distance |
| `R` | Retract – return to home (0 mm) |
| `M<val>` | Move to absolute position in mm (e.g. `M75.5`) |
| `S` | Stop motor immediately |
| `P` | Print current position in mm |
| `?` | Print command help |

## Configuration

Key parameters in `firmware/linear_actuator/config.h`:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `STEP_PIN` | 3 | Arduino pin for STEP signal |
| `DIR_PIN` | 4 | Arduino pin for DIR signal |
| `ENABLE_PIN` | 5 | Arduino pin for ENABLE signal |
| `LIMIT_SW_HOME_PIN` | 8 | Home limit switch pin |
| `LIMIT_SW_END_PIN` | 9 | End limit switch pin |
| `MICROSTEP_RESOLUTION` | 8 | Driver microstepping (match jumper setting) |
| `LEAD_SCREW_PITCH_MM` | 8.0 | Lead screw pitch in mm/revolution |
| `TOTAL_TRAVEL_MM` | 150.0 | Maximum actuator travel in mm |
| `MAX_SPEED_STEPS_PER_SEC` | 1600 | Maximum motor speed (steps/s) |

## License

This project is released under the [CC0 1.0 Universal](LICENSE) public domain dedication.
