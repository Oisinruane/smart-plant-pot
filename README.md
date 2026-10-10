# Smart Plant Pot

A plant pot that tells you when it needs watering.

A capacitive soil moisture sensor sits in the pot and is read once a second by a Wemos D1 mini (ESP8266). The moisture level is shown as a percentage on a small OLED display. When the soil gets too dry, a ring of 16 white LEDs around the base of the pot slowly fades in and out in a "breathing" pattern until the plant is watered. Over WiFi the pot also serves its current reading as JSON, so a home dashboard or script can keep track of it.

## Features

- **Live moisture reading:** percentage and bar graph on a 0.96" 128×64 OLED
- **Watering alert:** LED ring breathes below 30% moisture and stops above 40%, so it doesn't flicker at the threshold
- **JSON API:** one read-only endpoint with the current level
- **Over-the-air updates:** re-flash over WiFi after the first USB upload, protected by a password
- **Works offline:** leave the WiFi details blank and it runs as a standalone pot

## Hardware

| Part | Purpose |
|---|---|
| Wemos D1 mini (ESP8266) | Microcontroller and WiFi |
| Capacitive soil moisture sensor v1.2 (TLC555 version) | Measures soil moisture without corroding |
| 0.96" SSD1306 OLED, I2C, 128×64 | Shows the moisture percentage |
| 16 × white SMD LEDs, each with a 120 Ω resistor | Breathing ring at the base |
| AO3400A N-MOSFET, 100 Ω and 100 kΩ resistors | Switches the LED ring from a PWM pin |
| 470 µF capacitor | Smooths the LED ring's 5 V supply |

| D1 mini | Connects to |
|---|---|
| A0 | Sensor AOUT |
| D2 / D1 | OLED SDA / SCL |
| D5 | LED ring MOSFET gate |
| 5V | LED ring supply |
| 3V3 | OLED and sensor |

Full wiring, the LED ring design and the power budget are in [docs/wiring.md](docs/wiring.md).

## API

```bash
$ curl http://plant-pot.local/api/moisture
{"moisture_pct":54,"raw":437,"needs_water":false}
```

| Field | Meaning |
|---|---|
| `moisture_pct` | Calibrated moisture level, 0–100 |
| `raw` | Smoothed ADC reading (0–1023), used for calibration |
| `needs_water` | `true` while the LED ring is breathing |

## Getting started

Requires [PlatformIO](https://platformio.org/). On Apple Silicon Macs the ESP8266 toolchain also needs Rosetta (`softwareupdate --install-rosetta`).

```bash
cp src/secrets.example.h src/secrets.h   # WiFi details and OTA password; git-ignored
pio run -e d1_mini -t upload              # first flash over USB
pio device monitor                        # watch raw readings

export PLANT_POT_OTA_PASSWORD=...         # same as OTA_PASSWORD in secrets.h
pio run -e d1_mini_ota -t upload          # later flashes over WiFi
```

### Calibrating the sensor

Every sensor reads slightly differently. With the serial monitor open, note the `raw` value with the sensor in open air (dry) and in a glass of water (wet), then set `SENSOR_DRY_RAW` and `SENSOR_WET_RAW` in [src/config.h](src/config.h) and re-flash. Thresholds, breathing speed and LED brightness are in the same file.

## Tests

The calibration mapping, smoothing, watering hysteresis and breathing curve are hardware-free and tested on the host:

```bash
g++ -std=c++17 -Wall -I src tests/test_plant_logic.cpp -o /tmp/plant_tests && /tmp/plant_tests
```

## Documentation

- [docs/overview.md](docs/overview.md): project background, why the D1 mini, and folder layout
- [docs/wiring.md](docs/wiring.md): pin map, schematic sketch, LED ring and power notes
- [docs/process_flow.md](docs/process_flow.md): how data moves from the sensor to the display, LEDs and API
