# Hardware

This directory contains all hardware-related documentation and design files for the linear actuator used to deploy an NPK soil probe on a robotic platform.

## Contents

| File / Directory | Description |
|-----------------|-------------|
| [`BOM.md`](BOM.md) | Bill of Materials – component list with specifications |
| [`pinout.md`](pinout.md) | Pin-out table and wiring diagrams |
| [`schematics/`](schematics/) | Schematic and PCB design files (KiCad) |

## Mechanical Overview

The actuator uses a NEMA 17 stepper motor coupled to a T8 lead screw (8 mm pitch) via a flexible shaft coupler. A carriage riding on an MGN12 linear guide rail carries the NPK probe. Two micro-switch limit switches define the travel endpoints:

- **Home switch (LS1):** probe fully retracted / stowed position (0 mm).
- **End switch (LS2):** probe fully extended / deployed position (default 150 mm, adjustable in `firmware/linear_actuator/config.h`).

## Quick-Start Wiring

See [`pinout.md`](pinout.md) for detailed connection tables and ASCII wiring diagrams.

## Safety Notes

1. Always perform a homing sequence (`H` command) before issuing movement commands.
2. Do not exceed the rated current of the stepper motor driver.
3. Ensure the lead screw and carriage are free to move before powering the system.
4. Do not remove limit switches from the circuit – they are the primary over-travel protection.
