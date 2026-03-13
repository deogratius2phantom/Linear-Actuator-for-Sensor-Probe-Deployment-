# Pin-out and Wiring Reference

Linear Actuator – NPK Sensor Probe Deployment System

## Arduino Mega 2560 Pin Assignments

| Arduino Pin | Signal | Connected To | Description |
|-------------|--------|--------------|-------------|
| D3 | STEP | Driver STEP | Step pulse output to motor driver |
| D4 | DIR | Driver DIR | Direction signal to motor driver |
| D5 | EN | Driver EN | Enable signal (active LOW – LOW = enabled) |
| D8 | LIMIT_HOME | LS1 (NO contact) | Home / retracted limit switch |
| D9 | LIMIT_END | LS2 (NO contact) | End / extended limit switch |
| D13 | STATUS_LED | Onboard LED | Ready indicator (HIGH = homed and ready) |
| 5 V | VCC | Driver VCC, switches common | Logic power rail |
| GND | GND | Driver GND, switch return | Common ground |

> Pin numbers are configurable in `firmware/linear_actuator/config.h`.

## Motor Driver (A4988) Wiring

```
Arduino          A4988 Driver          NEMA 17 Motor
---------        ---------------       ---------------
D3 (STEP) -----> STEP
D4 (DIR)  -----> DIR
D5 (EN)   -----> ENABLE
5 V       -----> VCC (logic)
GND       -----> GND (logic)
                 VMOT <-------------- 12 V PSU (+)
                 GND  <-------------- 12 V PSU (-)
                 1A, 1B ------------> Coil A (motor wires)
                 2A, 2B ------------> Coil B (motor wires)
```

Place a 100 µF capacitor between VMOT and GND on the driver board as close to the driver as possible to protect against voltage spikes.

## Limit Switch Wiring (Normally-Open, pull-up)

```
Arduino D8 ──────┬────── LS1 (Home, NO) ──── GND
                 │
              10 kΩ (optional external pull-up, or use INPUT_PULLUP)
                 │
                5 V

Arduino D9 ──────┬────── LS2 (End, NO) ──── GND
                 │
              10 kΩ (optional external pull-up, or use INPUT_PULLUP)
                 │
                5 V
```

The firmware enables the Arduino's internal pull-up resistors (`INPUT_PULLUP`).  
Switch state when triggered: **LOW** (pin pulled to GND through closed switch contact).

## Power Supply Wiring

```
12 V PSU (+) ──── A4988 VMOT
             ──── Barrel jack (+) (optional; use a buck converter for Arduino Vin)
12 V PSU (-) ──── A4988 GND
             ──── Arduino GND (common ground)
```

> **Important:** Never connect 12 V directly to the Arduino 5 V pin. Use the Arduino's VIN pin (accepts 7–12 V) or a dedicated 5 V regulator.

## Schematic

See `hardware/schematics/` for the KiCad schematic source files and exported PDFs.
