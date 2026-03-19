# Bill of Materials (BOM)

Linear Actuator – NPK Sensor Probe Deployment System

| # | Reference | Component | Specification | Qty | Notes |
|---|-----------|-----------|---------------|-----|-------|
| 1 | U1 | Microcontroller | Arduino Mega 2560 (or Uno) | 1 | Main controller board |
| 2 | M1 | Stepper Motor | NEMA 17, 1.8°/step, 12 V, 1.5 A (e.g. 17HS4401) | 1 | 200 steps/rev |
| 3 | DRV1 | Stepper Motor Driver | A4988 or DRV8825 breakout board | 1 | Set Vref for motor current limit |
| 4 | LS1 | Home Limit Switch | Micro switch, SPDT, 1 A 250 VAC (e.g. SS-5GL) | 1 | Retracted / home position |
| 5 | LS2 | End Limit Switch | Micro switch, SPDT, 1 A 250 VAC (e.g. SS-5GL) | 1 | Extended / deployed position |
| 6 | PS1 | Power Supply | 12 V DC, ≥ 2 A | 1 | Supplies motor; use separate 5 V for Arduino |
| 7 | C1 | Electrolytic Capacitor | 100 µF, 25 V | 1 | Across motor driver VMOT and GND (see driver datasheet) |
| 8 | R1, R2 | Pull-up Resistors | 10 kΩ, 1/4 W (optional – internal pull-ups used in firmware) | 2 | External pull-ups for limit switch lines if needed |
| 9 | — | Lead Screw Assembly | T8 lead screw, 8 mm pitch, with brass nut and couplings | 1 | 150 mm travel (adjust TOTAL_TRAVEL_MM in config.h) |
| 10 | — | Linear Guide Rail | MGN12 or equivalent, ≥ 200 mm | 1 | Guides the probe carriage |
| 11 | — | Flexible Shaft Coupler | 5 mm to 8 mm bore, 25 mm length | 1 | Motor shaft to lead screw |
| 12 | — | Structural Frame | Aluminium extrusion 2020 or 3D-printed bracket | 1 set | Per mechanical design drawings |
| 13 | — | Wiring | 22 AWG stranded wire, various colours | — | Motor: 26 AWG twisted pair preferred |
| 14 | — | Connectors | JST-XH 2.54 mm, 4-pin (motor) and 2-pin (limit switches) | 3 | |

## Microstepping Jumper Settings (A4988)

| Resolution | MS1 | MS2 | MS3 |
|------------|-----|-----|-----|
| Full step  | L   | L   | L   |
| Half step  | H   | L   | L   |
| 1/4 step   | L   | H   | L   |
| 1/8 step   | H   | H   | L   |
| 1/16 step  | H   | H   | H   |

Default firmware configuration uses **1/8 microstepping** (`MICROSTEP_RESOLUTION = 8` in `config.h`).

## Driver Current Limit

Set the A4988 Vref potentiometer so that:

```
Vref = I_motor × 8 × R_sense
```

For the 17HS4401 (1.5 A rated, run at 1.2 A) with R_sense = 0.1 Ω:
```
Vref = 1.2 × 8 × 0.1 = 0.96 V
```

Measure Vref between the potentiometer wiper and GND with the driver powered and motor disconnected.
