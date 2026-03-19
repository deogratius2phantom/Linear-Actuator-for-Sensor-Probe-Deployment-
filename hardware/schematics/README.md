# Schematics

This directory contains the schematic and PCB design files for the linear actuator control board.

## Files

| File | Tool | Description |
|------|------|-------------|
| `linear_actuator.kicad_pro` | KiCad 7 | Project file |
| `linear_actuator.kicad_sch` | KiCad 7 | Schematic source |
| `linear_actuator.kicad_pcb` | KiCad 7 | PCB layout |
| `linear_actuator_schematic.pdf` | PDF export | Human-readable schematic |

> **Note:** Schematic source files (`.kicad_sch`, `.kicad_pcb`) will be added in a future commit once the PCB design is finalised. Until then, refer to `hardware/pinout.md` for full wiring details.

## Block Diagram

```
  ┌──────────────────────────────────────────────────┐
  │                Arduino Mega 2560                  │
  │                                                   │
  │  D3 (STEP) ───────────────────────────────────┐  │
  │  D4 (DIR)  ─────────────────────────────────┐ │  │
  │  D5 (EN)   ───────────────────────────────┐ │ │  │
  │                                           │ │ │  │
  └───────────────────────────────────────────┼─┼─┼──┘
                                              │ │ │
                                    ┌─────────▼─▼─▼──────────┐
                                    │      A4988 Driver        │
                                    │  VMOT ◄── 12 V PSU       │
                                    │  1A, 1B ──► NEMA 17      │
                                    │  2A, 2B ──► NEMA 17      │
                                    └──────────────────────────┘

  ┌─────────────────────────────────────────────┐
  │  D8 ◄── LS1 Home Limit Switch (NO, GND)    │
  │  D9 ◄── LS2 End  Limit Switch (NO, GND)    │
  └─────────────────────────────────────────────┘
```

## Recommended Tools

- [KiCad 7](https://www.kicad.org/) – Open-source EDA suite used for schematic capture and PCB layout.
- [FreeCAD](https://www.freecadweb.org/) – Mechanical design for the actuator frame and carriage.
