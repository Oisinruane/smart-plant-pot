# Overview

A plant pot that monitors its own soil moisture. A capacitive sensor in the soil is read by a Wemos D1 mini (ESP8266). The moisture level is shown as a percentage on a small OLED. When the soil dries out, a ring of 16 white LEDs at the base of the pot "breathes" until the plant is watered. The current reading is served as JSON over WiFi so the homelab dashboard can poll it.

## Why the D1 mini

The ESP-01 was the first choice but does not expose the ESP8266's ADC pin, so it would need an external ADC (ADS1115) to read the sensor. The D1 mini has A0 broken out with a 3.2 V divider, spare PWM pins and a USB-serial chip for easy flashing, in a similar footprint.

## Parts

| Part | Notes |
|---|---|
| Wemos D1 mini (ESP8266) | Brain, WiFi |
| Capacitive soil moisture sensor v1.2 | Prefer the TLC555 variant for 3.3 V |
| SSD1306 OLED, I2C, 128×64 | Address 0x3C. Change `OLED_HEIGHT` in `src/config.h` for a 128×32 |
| 16 × white SMD LED + 16 × 120 Ω | Ring PCB, same LEDs as the helping hand ring |
| AO3400A N-MOSFET, 100 Ω, 100 kΩ | Low-side PWM switch for the ring |
| 470 µF electrolytic | Bulk capacitance on the 5 V ring supply |

## Folder contents

| Path | Contents |
|---|---|
| `src/main.cpp` | Firmware: sensor sampling, OLED, LED breathing, JSON API, OTA |
| `src/plant_logic.h` | Hardware-free logic (calibration mapping, smoothing, hysteresis, breathing curve) |
| `src/config.h` | Pins, calibration and thresholds |
| `src/secrets.example.h` | WiFi credentials template (copy to `secrets.h`, which is git-ignored) |
| `tests/test_plant_logic.cpp` | Host unit tests for `plant_logic.h` |
| `platformio.ini` | Build config and library dependencies |
| `docs/wiring.md` | Pin map, schematic sketch, LED ring and power notes |
| `docs/process_flow.md` | How data moves from sensor to display, LEDs and network |

## Status

Breadboard prototype tested on a NodeMCU (sensor + OLED). Firmware written for the D1 mini. Next: build on the D1 mini, calibrate the sensor, then design the ring PCB.
